
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include <cstdlib>
#include <basicmsg_m.h>
#include <NodeClock.h>
#include <Neighbours.h>

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

        cMessage *event;    // pointer to the event object which will be used for timing
        cMessage *broadcast_tree; // variable to remember the message until its sent back

        virtual std::string parseNodeID(const char* nodeName);

        virtual BasicMessage* generateLeaderMessage();
        virtual BasicMessage* generateAckMessage();
        virtual BasicMessage* generateStartSpanningTreeMessage();
        virtual BasicMessage* generateSpanningTreeRequest();
        virtual BasicMessage* generateSpanningTreeAck();
        virtual BasicMessage* generateSpanningTreeDecline();
        virtual BasicMessage* generateSpanningTreeBroadCast();
        virtual BasicMessage* generateSpanningTreeBroadCastReply();

        virtual bool receivingAck();

    protected:
        virtual void initialize() override;
        virtual void initialize_parameters();

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
    if(false) {
        std::string id_string = this->parseNodeID(getFullName());
        node_id = std::stoi(id_string);
    } else {
        node_id = rand();
    }

    EV << "Node " << getFullName() << " has id: " << node_id << "\n";

}

void BasicNode::broadcastLeaderRequest()
{
    for (int i = 0; i < 6; i++) {
        BasicMessage *msg = generateLeaderMessage();
        send(msg, "out", i);
    }
}

void BasicNode::broadcastStartSpanningTree()
{
    // Start spanningtree search
    for (int i = 0; i < 6; i++) {
        BasicMessage *msg = generateStartSpanningTreeMessage();
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
        BasicMessage *msg = generateSpanningTreeBroadCast();
        EV << "Sending message down the tree to: " << child_nodes[i];
        send(msg, "out", child_nodes[i]);
        node_messages_sent++;
    }

    // Restart timer to retry if it fails
    if (leader) {
        event = new cMessage("event");
        broadcast_tree = new cMessage("broadcast_tree");
        scheduleAt((simTime() + 10.0), event);
    }

}

void BasicNode::replyTreeBroadcast(int out_gate)
{
    BasicMessage *msg = generateSpanningTreeBroadCastReply();
    send(msg, "out", out_gate);
}

void BasicNode::spanningTreeRequest()
{
    int requests = 2;

    int *gate_request_list = connected_neighbours.get_random_gates(requests);

    for (int i = 0; i < requests; i++)
    {
        BasicMessage *msg = generateSpanningTreeRequest();
        send(msg, "out", *(gate_request_list + i));
    }

    spanning_request_answer = 0;
}

void BasicNode::acceptSpanningTreeRequest(int arrival_gate)
{
    BasicMessage *msg = generateSpanningTreeAck();
    send(msg, "out", arrival_gate);
}

void BasicNode::declineSpanningTreeRequest(int arrival_gate)
{
    BasicMessage *msg = generateSpanningTreeDecline();
    send(msg, "out", arrival_gate);
}

void BasicNode::handleMessage(cMessage *msg)
{

    // https://stackoverflow.com/questions/40873629/omnet-adding-functionalities-to-handlemessage-in-my-class
    if (msg == event)
    {
        EV << "Node: " << getFullName() << " is going to start the down tree broadcast\n has number of child nodes: " << num_of_child_nodes << "\n";
        broadcastDownSpanningTree();
        clock.increment_time();
        delete msg;
        return;
    }

    BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (msg);
    EV << "Received Message: " << basicmsg->getArrivalGate()->getIndex() << " with timestamp: " << basicmsg->getScalar_clock();

    int incoming_node_id = basicmsg->getSrc_node_id();
    int arrival = msg->getArrivalGate()->getIndex();
    bool isMsgAck = basicmsg->getAck();

    bool start_spanning_tree_search = basicmsg->getStart_spanning_tree();
    bool spanning_request = basicmsg->getSpanning_request();
    bool spanning_request_ack = basicmsg->getSpanning_request_ack();
    bool spanning_decline = basicmsg->getSpanning_decline_request();
    bool down_request = basicmsg->getDown_broadcast();
    bool up_request = basicmsg->getUp_broadcast_reply();

    EV << "Down_request: " << down_request << "\n";

    if (spanning_decline) {
        spanning_request_answer++;
        if ((!part_of_tree) && (spanning_request_answer >= 2)){
            spanningTreeRequest();
        }
    } else if (spanning_request_ack) {
        spanning_request_answer++;
        if (!part_of_tree) {
            part_of_tree = true;
            parent_node = arrival;
            tree_level = basicmsg->getSpanning_tree_level() + 1;
            EV << "Node: " << getFullName()<< " has joined the tree at level: " << tree_level << " parent node is: " << arrival << "\n";
        }

    } else if (down_request) {
        EV << "Replying to down request\n";
        node_messages_sent = 0;
        node_answers = 0;
        down_request_gate = arrival;
        if(num_of_child_nodes > 0) {
            broadcastDownSpanningTree();
        } else {
            replyTreeBroadcast(arrival);
        }
    } else if (up_request) {
        node_answers++;
        if (node_answers == node_messages_sent) {
            if (!leader){
                replyTreeBroadcast(down_request_gate);
            } else {
                EV << "Broadcast down tree was complete";
            }
        }
    } else if (spanning_request) {
        if ((part_of_tree) && (num_of_child_nodes < 2)){
            acceptSpanningTreeRequest(arrival);
            child_nodes[num_of_child_nodes] = arrival;
            num_of_child_nodes++;
        } else {
            declineSpanningTreeRequest(arrival);
        }
    } else if (start_spanning_tree_search) {
        spanningTreeRequest();

        event = new cMessage("event");
        broadcast_tree = new cMessage("broadcast_tree");
        scheduleAt((simTime() + 10.0), event);

    } else if ((!isMsgAck) && (node_id < incoming_node_id)) {
        BasicMessage * ack_msg = generateAckMessage();
        send(ack_msg, "out", arrival);
    } else if (isMsgAck) {
        if (receivingAck()) {
            broadcastStartSpanningTree();
            part_of_tree = true;
            tree_level = 0;
        }
    }

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

BasicMessage* BasicNode::generateLeaderMessage()
{
    clock.increment_time();

    int src = getIndex();

    char msgname[40];
    sprintf(msgname, "Message sent from node: %d", src);

    // Creating message
    BasicMessage *msg = new BasicMessage(msgname);
    msg->setSource(src);

    EV << "Scalar clock for src: " << src << " is currently: " << clock.get_scalar_time() << "\n";

    msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);

    ack_counter++;

    return msg;
}

BasicMessage * BasicNode::generateAckMessage()
{
    clock.increment_time();

    char msgname[120];
    sprintf(msgname, "Ack from: %d", node_id);

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(true);

    return msg;
}

BasicMessage * BasicNode::generateStartSpanningTreeMessage()
{
    clock.increment_time();

    char msgname[120];
    sprintf(msgname, "Leader has been elected: %d. Starting Spanning tree process", node_id);

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);

    msg->setStart_spanning_tree(true);
    msg->setRoot_node(node_id);

    return msg;
}

BasicMessage * BasicNode::generateSpanningTreeRequest()
{
    clock.increment_time();

    char msgname[40];
    sprintf(msgname, "Requesting to join spanning tree");

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);

    msg->setSpanning_request(true);

    return msg;
}

BasicMessage * BasicNode::generateSpanningTreeAck()
{
    clock.increment_time();

    char msgname[40];
    sprintf(msgname, "Accept child");

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);

    msg->setSpanning_request_ack(true);
    msg->setSpanning_tree_level(tree_level);

    return msg;
}

BasicMessage * BasicNode::generateSpanningTreeDecline()
{
    clock.increment_time();

    char msgname[40];
    sprintf(msgname, "Decline child");

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);

    msg->setSpanning_decline_request(true);

    return msg;
}

BasicMessage * BasicNode::generateSpanningTreeBroadCast()
{
    clock.increment_time();

    char msgname[40];
    sprintf(msgname, "Broadcast request through tree");

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);

    msg->setDown_broadcast(true);

    return msg;
}

BasicMessage * BasicNode::generateSpanningTreeBroadCastReply()
{
    clock.increment_time();

    char msgname[40];
    sprintf(msgname, "Reply to broadcast request");

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);

    msg->setUp_broadcast_reply(true);

    return msg;
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
