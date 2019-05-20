
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

#include "src/transaction/Transaction.h"

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

        Transaction transaction;

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

    transaction.set_connected_neighbours(&connected_neighbours);
    transaction.set_node_id(node_id);

    start_message_timer();

}

void BasicNode::start_message_timer()
{
    event = new cMessage("event");
    // broadcast_tree = new cMessage("broadcast_tree");
    scheduleAt((simTime() + 0.05), event);
}

void BasicNode::sendMessagesFromBuffer(void)
{
    int msg_in_leader_buffer = leader_election.getMessageCount();

    // EV << "Number of messages in leader buffer: " << msg_in_leader_buffer << "\n";

    for (int i = 0; i < msg_in_leader_buffer; i++)
    {
        BufferedMessage * buf_msg = leader_election.getMessage();
        // EV << "src: " << node_id << " is sending a message to: " << buf_msg->get_out_gate_int() << "\n";
        send(buf_msg->get_message(), "out", buf_msg->get_out_gate_int());
        delete(buf_msg);
    }

    // EV << "Number of messages in leader buffer after sending messages: " << leader_election.getMessageCount() << "\n";

    int msgSpanningTrees = spanning_trees.get_message_count();

    for (int i = 0; i < msgSpanningTrees; i++)
    {
        BufferedMessage * bufMsg = spanning_trees.get_message();

        BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (bufMsg->get_message());
        EV << node_id << ": SpanningTree sending message type: " << basicmsg->getSubType() << " to address: " << bufMsg->get_out_gate_int() << " \n";
        sendDelayed(bufMsg->get_message(), bufMsg->get_delay(), "out", bufMsg->get_out_gate_int());
        delete(bufMsg);
    }

    int msgTransaction = transaction.get_message_count();

    for (int i = 0; i < msgTransaction; i++) {
        BufferedMessage * bufMsg = transaction.get_message();

        BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (bufMsg->get_message());
        sendDelayed(bufMsg->get_message(), bufMsg->get_delay(), "out", bufMsg->get_out_gate_int());
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
        start_message_timer();

        if(spanning_trees.full_broadcast_finished()) {
            spanning_trees.update_linked_nodes(connected_neighbours.get_linked_nodes());
            EV << "Node: " << node_id << "\n Neighbours: \n" << connected_neighbours.to_string() << "\n \n \n";
        }

        if(spanning_trees.full_broadcast_finished() and spanning_trees.is_node_root() and (transaction.get_current_transaction_id() < 0)) {
            int send = transaction.send(21, 10);
            EV << "Node 19 will send to" << send << "\n";
//            if(send) {
//                EV << "Node 19 should be sending a message \n";
//            } else {
//                EV << "Node 19 is not sending a message \n";
//            }
        }

        return;
    }

    BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (msg);

    EV << "Node " << getFullName() << " has id: " << node_id << " and  sent requests: " << spanning_trees.get_num_sent_messages() << "\n";

    if(basicmsg->getType() == LeaderElection::LEADER_MSG) {
        leader_election.handleMessage(basicmsg, msg->getArrivalGate()->getIndex());
    } else if(basicmsg->getType() == SpanningTree::MESSAGE_TYPE){
        spanning_trees.handle_message(basicmsg, msg->getArrivalGate()->getIndex(), simTime());
    } else if(basicmsg->getType() == Transaction::MESSAGE_TYPE) {
        transaction.handle_message(basicmsg, msg->getArrivalGate()->getIndex());
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
