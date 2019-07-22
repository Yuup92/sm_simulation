
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

        bool firstSend;
        bool startSimulation;

        int pulseTree;

        // NUM of trees must be equal to the number of SpanningTree and Neighbours
        static const int NUM_OF_TREES = 10;
        SpanningTree spanning_trees[10];
        Neighbours connected_neighbours[10];

        // Initialize the list of capacity
        // Ensure that enough is buffered
        LinkCapacity linkCapacities[30];


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

    startSimulation = false;
    pulseTree = 0;

    // Neighbours
    for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
        connected_neighbours[i].set_number_of_neighbours(gateSize("out"));

        spanning_trees[i].set_node_id(nodeId);
        spanning_trees[i].set_spanning_tree_index(i);
        spanning_trees[i].set_neighbours(&connected_neighbours[i]);

        // EV << "Node " << getFullName() << " has id: " << nodeId << " and  numneighbours: " << spanning_trees.get_num_neighbours() << "\n";

    }

    // TODO get transactions to work with connected neighbours
     transaction.set_connected_neighbours(connected_neighbours);
     transaction.set_node_id(nodeId);



     //TODO
     // This can be done differently, initializing linkedCapacity
     for(int i = 0; i < 23; i++) {
         linkCapacities[i].set_connected_node_id(i);
         linkCapacities[i].set_current_capacity(100);
     }



    // EV << "Node " << getFullName() << " has id: " << nodeId << " and  numneighbours: " << connected_neighbours.get_amount_of_neighbours() << "\n";

//    leader_election.setId(nodeId);
//    leader_election.setAmountNeighbours(connected_neighbours.get_amount_of_neighbours());

    start_message_timer();
    firstSend = false;
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

        // Finish Initialization process

        if(simTime() > 200 and not startSimulation) {
            for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
                spanning_trees[i].update_linked_nodes(connected_neighbours[i].get_linked_nodes());
                connected_neighbours[i].add_capacities_to_linked_nodes(&linkCapacities[0], 23);
            }
            startSimulation = true;
        }

        //
        // Start the random simulation here
        //

        if(startSimulation) {
            int sendTransactionProbability = rand();

            // 0.05 % chance of sending
            if(sendTransactionProbability < 10000000) {
                int amountProbability = rand() % 100;
                int amount = 1;

                if(amountProbability < 5) {
                    amount = 20;
                } else if( amountProbability < 30) {
                    amount = 10;
                } else if( amountProbability < 60) {
                    amount = 5;
                }

                int nodeProbability = rand() % 100;
                int receiver = 0;

                if(nodeProbability < 5) {
                    if(nodeProbability % 2 == 0) {
                        receiver = nodeId - 2;
                    } else {
                        receiver = nodeId + 2;
                    }
                } else if(nodeProbability < 25) {
                    if(nodeProbability % 2 == 0) {
                        receiver = nodeId - 3;
                    } else {
                        receiver = nodeId + 3;
                    }
                } else if(nodeProbability < 45) {
                    if(nodeProbability % 2 == 0) {
                        receiver = nodeId - 4;
                    } else {
                        receiver = nodeId + 4;
                    }
                } else if(nodeProbability < 65) {
                    if(nodeProbability % 2 == 0) {
                        receiver = nodeId - 5;
                    } else {
                        receiver = nodeId + 5;
                    }
                } else if(nodeProbability < 85) {
                    if(nodeProbability % 2 == 0) {
                        receiver = nodeId - 6;
                    } else {
                        receiver = nodeId + 6;
                    }
                } else if(nodeProbability < 95) {
                    if(nodeProbability % 2 == 0) {
                        receiver = nodeId - 7;
                    } else {
                        receiver = nodeId + 7;
                    }
                } else if(nodeProbability < 100) {
                    if(nodeProbability % 2 == 0) {
                        receiver = nodeId - 8;
                    } else {
                        receiver = nodeId + 8;
                    }
                }

                if(receiver < 0) {
                    receiver = nodeId - receiver;
                }

                if(receiver > 22) {
                    receiver = receiver - nodeId;
                }

                EV << "Node: " << nodeId << " \nSending Amount: " << amount << " \nReceiver: " << receiver << "\n";

                int send = transaction.send(receiver, amount);

            }

            if(transaction.get_current_transaction_index() > 1) {

                EV << "node: " << nodeId << " has transaction string with capacities: " << transaction.capacities_to_string() << " \n delay:" << transaction.delay_to_string() << "\n";

            }
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
