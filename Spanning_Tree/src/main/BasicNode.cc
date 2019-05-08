
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <cstdlib>

#include "src/msg/basicmsg_m.h"
#include "src/sync/NodeClock.h"
#include "src/connection/Neighbours.h"

#include "src/msg/outgoing_buf/BufferedMessage.h"

#include "src/alg/leader_election/LeaderElection.h"
#include "src/alg/spanning_tree/SpanningTree.h"

using namespace omnetpp;

class BasicNode : public cSimpleModule
{
    private:
        NodeClock clock;
        int node_id;
        int ack_counter;
        bool leader;

        int pulseTree;

        Neighbours connected_neighbours;
        LeaderElection leader_election;
        SpanningTree spanning_trees;

        cMessage *event;    // pointer to the event object which will be used for timing
        cMessage *broadcast_tree; // variable to remember the message until its sent back

        virtual void sendMessagesFromBuffer(void);

        virtual std::string parseNodeID(const char* nodeName);

    protected:
        virtual void initialize() override;
        virtual void initialize_parameters();

        virtual void start_message_timer();

        virtual void handleMessage(cMessage *msg) override;

        // Starts the search for a leader
        virtual void broadcastLeaderRequest();

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
    // broadcastLeaderRequest();
    spanning_trees.wake_up();
}

void BasicNode::initialize_parameters()
{

    // Neighbours
    connected_neighbours = Neighbours(gateSize("out"));



    // Get node ID or generate randomly
    if(true) {
        std::string id_string = this->parseNodeID(getFullName());
        node_id = std::stoi(id_string);
    } else {
        node_id = rand();
    }

    EV << "Node " << getFullName() << " has id: " << node_id << " and  numneighbours: " << connected_neighbours.get_amount_of_neighbours() << "\n";

    leader_election.setId(node_id);
    leader_election.setAmountNeighbours(connected_neighbours.get_amount_of_neighbours());

    pulseTree = 0;
    spanning_trees.set_node_id(node_id);
    spanning_trees.set_neighbours(&connected_neighbours);

    EV << "Node " << getFullName() << " has id: " << node_id << " and  numneighbours: " << spanning_trees.get_num_neighbours() << "\n";


    start_message_timer();

}

void BasicNode::start_message_timer()
{
    event = new cMessage("event");
    // broadcast_tree = new cMessage("broadcast_tree");
    scheduleAt((simTime() + 1.0), event);
}

void BasicNode::sendMessagesFromBuffer(void)
{
    int msg_in_leader_buffer = leader_election.getMessageCount();

    // EV << "Number of messages in leader buffer: " << msg_in_leader_buffer << "\n";

    for (int i = 0; i < msg_in_leader_buffer; i++)
    {
        BufferedMessage * buf_msg = leader_election.getMessage();
        // EV << "src: " << node_id << " is sending a message to: " << buf_msg->getOutGateInt() << "\n";
        send(buf_msg->getMessage(), "out", buf_msg->getOutGateInt());
        delete(buf_msg);
    }

    // EV << "Number of messages in leader buffer after sending messages: " << leader_election.getMessageCount() << "\n";

    int msgSpanningTrees = spanning_trees.get_message_count();

    for (int i = 0; i < msgSpanningTrees; i++)
    {
        BufferedMessage * bufMsg = spanning_trees.get_message();

        BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (bufMsg->getMessage());
        EV << node_id << ": SpanningTree sending message type: " << basicmsg->getSubType() << " to address: " << bufMsg->getOutGateInt() << " \n";
        send(bufMsg->getMessage(), "out", bufMsg->getOutGateInt());
        delete(bufMsg);
    }

}

void BasicNode::broadcastLeaderRequest()
{
    leader_election.broadcastLeaderRequest();
}


void BasicNode::handleMessage(cMessage *msg)
{

    // https://stackoverflow.com/questions/40873629/omnet-adding-functionalities-to-handlemessage-in-my-class
    if (msg == event)
    {
        delete msg;
        sendMessagesFromBuffer();
        spanning_trees.check_queued_messages();
        // EV << "Node: " << getFullName() << " has level: " << spanning_trees.get_level() << "\n";

        EV << "Node: " << getFullName() << " has edge state: " << spanning_trees.get_state_edge() << "\n";

        start_message_timer();

        if(simTime() == 100) {
            spanning_trees.check_root();
        } else if (simTime() == 105) {
            spanning_trees.broadcast();
        }

        return;
    }

    BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (msg);
    // EV << "Received Message: " << basicmsg->getArrivalGate()->getIndex() << " with timestamp: " << basicmsg->getScalar_clock() << " \n";

    spanning_trees.handle_message(basicmsg, msg->getArrivalGate()->getIndex());



    EV << "Node " << getFullName() << " has id: " << node_id << " and  sent requests: " << spanning_trees.get_num_sent_messages() << "\n";
//    if(basicmsg->getType() == MessageType::get_leader_message_type()) {
//        leader_election.handleMessage(basicmsg, msg->getArrivalGate()->getIndex());
//    } else if(basicmsg->getType() == MessageType::get_spanning_tree_message_type()){
//        spanning_trees.handle_message(basicmsg, msg->getArrivalGate()->getIndex());
//    }

    if (leader_election.isLeader() and spanning_trees.get_state() == SpanningTree::STATE_SLEEPING)
    {
        spanning_trees.wake_up();
        EV << "I am the captain now: " << node_id << " \n";
    }



    delete basicmsg;
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
