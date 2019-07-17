
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
        int nodeId;
        int ack_counter;
        bool leader;

        int pulseTree;

        // NUM of trees must be equal to the number of SpanningTree and Neighbours
        static const int NUM_OF_TREES = 10;
        SpanningTree spanning_trees[10];
        Neighbours connected_neighbours[10];


        LeaderElection leader_election;
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
    for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
        spanning_trees[i].wake_up();
    }
}

void BasicNode::initialize_parameters()
{
    // Get node ID or generate randomly
    if(true) {
        std::string id_string = this->parseNodeID(getFullName());
        nodeId = std::stoi(id_string);
    } else {
        nodeId = rand();
    }

    pulseTree = 0;

    // Neighbours
    for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
        connected_neighbours[i] = Neighbours(gateSize("out"));

        spanning_trees[i].set_node_id(nodeId);
        spanning_trees[i].set_spanning_tree_index(i);
        spanning_trees[i].set_neighbours(&connected_neighbours[i]);

        // EV << "Node " << getFullName() << " has id: " << nodeId << " and  numneighbours: " << spanning_trees.get_num_neighbours() << "\n";

    }

    // TODO get transactions to work with connected neighbours
     transaction.set_connected_neighbours(connected_neighbours);
     transaction.set_node_id(nodeId);




    // EV << "Node " << getFullName() << " has id: " << nodeId << " and  numneighbours: " << connected_neighbours.get_amount_of_neighbours() << "\n";

//    leader_election.setId(nodeId);
//    leader_election.setAmountNeighbours(connected_neighbours.get_amount_of_neighbours());

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
        // EV << "src: " << nodeId << " is sending a message to: " << buf_msg->get_out_gate_int() << "\n";
        send(buf_msg->get_message(), "out", buf_msg->get_out_gate_int());
        delete(buf_msg);
    }

    // EV << "Number of messages in leader buffer after sending messages: " << leader_election.getMessageCount() << "\n";

    for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
        int msgSpanningTrees = spanning_trees[i].get_message_count();

        for (int j = 0; j < msgSpanningTrees; j++)
        {
            BufferedMessage * bufMsg = spanning_trees[i].get_message();

            BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (bufMsg->get_message());
            EV << nodeId << ": SpanningTree sending message type: " << basicmsg->getSubType() << " to address: " << bufMsg->get_out_gate_int() << " \n";
            sendDelayed(bufMsg->get_message(), bufMsg->get_delay(), "out", bufMsg->get_out_gate_int());
            delete(bufMsg);
        }
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
        for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
            spanning_trees[i].check_queued_messages();
        }

        start_message_timer();

        bool start_broadcast = true;

        for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
            if(spanning_trees[i].full_broadcast_finished() == false) {
                start_broadcast = false;
            } else if(spanning_trees[i].full_broadcast_finished() and (not spanning_trees[i].get_linked_nodes_updated())) {
                spanning_trees[i].update_linked_nodes(connected_neighbours[i].get_linked_nodes());
            }
        }

//        if(spanning_trees.full_broadcast_finished()) {
//            spanning_trees.update_linked_nodes(connected_neighbours.get_linked_nodes());
//
//            if(nodeId == 19 or nodeId == 20 or nodeId == 21 or nodeId == 22 or nodeId == 12) {
//                EV << "Node: " << nodeId << "\n Neighbours: \n" << connected_neighbours.to_string() << "\n \n \n";
//            }
//        }

        if((transaction.get_current_transaction_index() < 1)
                and nodeId == 1
                and (simTime() > 200)) {

            int send = transaction.send(21, 2);
            EV << "node: " << nodeId << " neighbour string: \n " << connected_neighbours[0].to_string() << "\n";

//
            EV << "\n\n Node " << nodeId <<" has the send string: " << send << "\n transaction to string: " << transaction.to_string() << "\n" ;

//            if(send) {
//                EV << "Node " << nodeId <<" should be sending a message \n is current transaction index: " << transaction.get_current_transaction_index() << "\n";
//            } else {
//                EV << "Node " << nodeId <<" is not sending a message \n";
//            }
        }

        return;
    }

    BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (msg);

 //   EV << "Node " << getFullName() << " has id: " << nodeId << " and  sent requests: " << spanning_trees.get_num_sent_messages() << "\n";

    if(basicmsg->getType() == LeaderElection::LEADER_MSG) {
        leader_election.handleMessage(basicmsg, msg->getArrivalGate()->getIndex());
    } else if(basicmsg->getType() == SpanningTree::MESSAGE_TYPE){
        spanning_trees[basicmsg->getSpanningTreeIndex()].handle_message(basicmsg, msg->getArrivalGate()->getIndex(), simTime());
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
