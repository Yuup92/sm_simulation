
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include <cstdlib>
#include <basicmsg_m.h>
#include <NodeClock.h>
#include <Neighbours.h>

#include <MessageBuffer.h>
#include <BufferedMessage.h>
#include <MessageGenerator.h>
#include <LeaderElection.h>

#include <MessageType.h>

using namespace omnetpp;

class BasicNode : public cSimpleModule
{
    private:
        NodeClock clock;
        int node_id;
        int ack_counter;
        bool leader;

        // Spanning tree Variables
        bool part_of_tree;
        int tree_level;
        int parent_node;
        int child_nodes[2];
        int num_of_child_nodes;
        int spanning_request_answer;

        int node_answers;
        int node_messages_sent;
        int down_request_gate;

        Neighbours connected_neighbours;
        LeaderElection leader_election;

        MessageBuffer message_buffer;

        cMessage *event;    // pointer to the event object which will be used for timing
        cMessage *broadcast_tree; // variable to remember the message until its sent back

        virtual void sendMessagesFromBuffer(void);

        virtual std::string parseNodeID(const char* nodeName);

        virtual bool receivingAck();

    protected:
        virtual void initialize() override;
        virtual void initialize_parameters();

        virtual void start_message_timer();

        virtual void handleMessage(cMessage *msg) override;

        // Starts the search for a leader
        virtual void broadcastLeaderRequest();

        // Starts the search for a spanning tree
        virtual void broadcastStartSpanningTree();

        // Broadcasts messages down the tree, could be used for heart beat
        virtual void broadcastDownSpanningTree();
        virtual void replyTreeBroadcast(int out_gate);

        virtual void spanningTreeRequest();
        virtual void acceptSpanningTreeRequest(int arrival_gate);
        virtual void declineSpanningTreeRequest(int arrival_gate);

    public:
        BasicNode();
        virtual ~BasicNode();
};

Define_Module(BasicNode);

// TODO sendMessageBuffer periodically?
BasicNode::BasicNode()
{
    // Set timer pointer to nullptr
    event = broadcast_tree = nullptr;
}

BasicNode::~BasicNode()
{
    cancelAndDelete(event);
    delete broadcast_tree;
}

void BasicNode::initialize()
{
    initialize_parameters();
    broadcastLeaderRequest();
}

void BasicNode::initialize_parameters()
{
    // Leader election
    leader = false;
    ack_counter = 0;

    // Spanning Tree
    part_of_tree = false;
    tree_level = -1;
    num_of_child_nodes = 0;
    spanning_request_answer = 0;

    // Neighbours
    connected_neighbours = Neighbours(gateSize("out"));

    // Get node ID or generate randomly
    if(true) {
        std::string id_string = this->parseNodeID(getFullName());
        node_id = std::stoi(id_string);
    } else {
        node_id = rand();
    }

    EV << "Node " << getFullName() << " has id: " << node_id << "\n";

    leader_election.setMsgBuf(message_buffer);
    leader_election.setId(node_id);
    leader_election.setAmountNeighbours(connected_neighbours.get_amount_of_neighbours());

    start_message_timer();

}

void BasicNode::start_message_timer()
{
    event = new cMessage("event");
    broadcast_tree = new cMessage("broadcast_tree");
    scheduleAt((simTime() + 1.0), event);
}

void BasicNode::sendMessagesFromBuffer(void)
{
    int msg_in_leader_buffer = leader_election.getMessageCount();

    EV << "Number of messages in leader buffer: " << msg_in_leader_buffer << "\n";

    for (int i = 0; i < msg_in_leader_buffer; i++)
    {
        BufferedMessage * buf_msg = leader_election.getMessage();
        EV << "src: " << node_id << " is sending a message to: " << buf_msg->getOutGateInt() << "\n";
        send(buf_msg->getMessage(), "out", buf_msg->getOutGateInt());
        delete(buf_msg);
    }

    EV << "Number of messages in leader buffer after sending messages: " << leader_election.getMessageCount() << "\n";
}

void BasicNode::broadcastLeaderRequest()
{
    leader_election.broadcastLeaderRequest();
}

void BasicNode::broadcastStartSpanningTree()
{
    // Start Spanningtree search
    for (int i = 0; i < 6; i++) {
        BasicMessage *msg = MessageGenerator::generateStartSpanningTreeMessage(node_id);
        send(msg, "out", i);
    }

    // Start timer to send messages down tree
    if (leader) {
        event = new cMessage("event");
        broadcast_tree = new cMessage("broadcast_tree");
        scheduleAt((simTime() + 10.0), event);
    }


}

void BasicNode::broadcastDownSpanningTree()
{

    // Send messages along the tree
    for (int i = 0; i < num_of_child_nodes; i++) {
        BasicMessage *msg = MessageGenerator::generateSpanningTreeBroadCast(node_id);
        EV << "Sending message down the tree to: " << child_nodes[i];
        send(msg, "out", child_nodes[i]);
        node_messages_sent++;
    }

    // Restart timer to retry if it fails
    if (leader) {
        event = new cMessage("event");
        broadcast_tree = new cMessage("broadcast_tree");
        scheduleAt((simTime() + 1.0), event);
    }

}

void BasicNode::replyTreeBroadcast(int out_gate)
{
    BasicMessage *msg = MessageGenerator::generateSpanningTreeBroadCastReply(node_id);
    send(msg, "out", out_gate);
}

void BasicNode::spanningTreeRequest()
{
    int requests = 2;

    int *gate_request_list = connected_neighbours.get_random_gates(requests);

    for (int i = 0; i < requests; i++)
    {
        BasicMessage *msg = MessageGenerator::generateSpanningTreeRequest(node_id);
        send(msg, "out", *(gate_request_list + i));
    }

    spanning_request_answer = 0;
}

void BasicNode::acceptSpanningTreeRequest(int arrival_gate)
{
    BasicMessage *msg = MessageGenerator::generateSpanningTreeAck(node_id, tree_level);
    send(msg, "out", arrival_gate);
}

void BasicNode::declineSpanningTreeRequest(int arrival_gate)
{
    BasicMessage *msg = MessageGenerator::generateSpanningTreeDecline(node_id);
    send(msg, "out", arrival_gate);
}

void BasicNode::handleMessage(cMessage *msg)
{

    // https://stackoverflow.com/questions/40873629/omnet-adding-functionalities-to-handlemessage-in-my-class
    if (msg == event)
    {
        sendMessagesFromBuffer();
        EV << "Node: " << getFullName() << " sending messages from buffer from timer" << "\n";
        delete msg;
        start_message_timer();
        return;
    }

    BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (msg);
    EV << "Received Message: " << basicmsg->getArrivalGate()->getIndex() << " with timestamp: " << basicmsg->getScalar_clock() << " \n";

    if(basicmsg->getType() == MessageType::get_leader_message_type()) {
        leader_election.handleMessage(basicmsg, msg->getArrivalGate()->getIndex());
    }

    if (leader_election.isLeader())
    {
        EV << "I am the captain now: " << node_id << " \n";
    }


//    int incoming_node_id = basicmsg->getSrc_node_id();
//    int arrival = msg->getArrivalGate()->getIndex();
//    bool isMsgAck = basicmsg->getAck();
//
//    bool start_spanning_tree_search = basicmsg->getStart_spanning_tree();
//    bool spanning_request = basicmsg->getSpanning_request();
//    bool spanning_request_ack = basicmsg->getSpanning_request_ack();
//    bool spanning_decline = basicmsg->getSpanning_decline_request();
//    bool down_request = basicmsg->getDown_broadcast();
//    bool up_request = basicmsg->getUp_broadcast_reply();
//
//    EV << "Down_request: " << down_request << "\n";
//
//    if (spanning_decline) {
//        spanning_request_answer++;
//        if ((!part_of_tree) && (spanning_request_answer >= 2)){
//            spanningTreeRequest();
//        }
//    } else if (spanning_request_ack) {
//        spanning_request_answer++;
//        if (!part_of_tree) {
//            part_of_tree = true;
//            parent_node = arrival;
//            tree_level = basicmsg->getSpanning_tree_level() + 1;
//            EV << "Node: " << getFullName()<< " has joined the tree at level: " << tree_level << " parent node is: " << arrival << "\n";
//        }
//
//    } else if (down_request) {
//        EV << "Replying to down request\n";
//        node_messages_sent = 0;
//        node_answers = 0;
//        down_request_gate = arrival;
//        if(num_of_child_nodes > 0) {
//            broadcastDownSpanningTree();
//        } else {
//            replyTreeBroadcast(arrival);
//        }
//    } else if (up_request) {
//        node_answers++;
//        if (node_answers == node_messages_sent) {
//            if (!leader){
//                replyTreeBroadcast(down_request_gate);
//            } else {
//                EV << "Broadcast down tree was complete";
//            }
//        }
//    } else if (spanning_request) {
//        if ((part_of_tree) && (num_of_child_nodes < 2)){
//            acceptSpanningTreeRequest(arrival);
//            child_nodes[num_of_child_nodes] = arrival;
//            num_of_child_nodes++;
//        } else {
//            declineSpanningTreeRequest(arrival);
//        }
//    } else if (start_spanning_tree_search) {
//        spanningTreeRequest();
//
//        event = new cMessage("event");
//        broadcast_tree = new cMessage("broadcast_tree");
//        scheduleAt((simTime() + 10.0), event);
//
//    } else if ((!isMsgAck) && (node_id < incoming_node_id)) {
//        BasicMessage * ack_msg = MessageGenerator::generateAckMessage(node_id);
//        send(ack_msg, "out", arrival);
//    } else if (isMsgAck) {
//        if (receivingAck()) {
//            broadcastStartSpanningTree();
//            part_of_tree = true;
//            tree_level = 0;
//        }
//    }

    delete basicmsg;

    clock.increment_time(); // ? not sure if I should increment time here, depends on if I think accepting a message is an event
}

bool BasicNode::receivingAck()
{
    ack_counter--;

    if (ack_counter == 0)
    {
        EV << "I am the captain now";
        leader = true;
    }

    return leader;

}

// based on https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
// Parses the Node_#ID and returns a string of #ID
std::string BasicNode::parseNodeID(const char* nodeName)
{
   std::string node_ID = nodeName;
   std::string delimiter = "_";

   size_t pos = 0;

   pos = node_ID.find(delimiter);

   node_ID.erase(0, pos + delimiter.length());

   return node_ID;
}
