
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

#include "src/output/FileWriter.h"
#include "src/output/SaveState.h"

using namespace omnetpp;

class BasicNode : public cSimpleModule
{
    public:
        static const int NUM_OF_TREES = 10;
        static const int NUM_OF_NODES = 23;

        BasicNode();
        virtual ~BasicNode();

    protected:
        virtual void initialize() override;
        virtual void initialize_parameters();

        virtual void start_message_timer();

        virtual void handleMessage(cMessage *msg) override;

        // Starts the search for a leader
        virtual void broadcastLeaderRequest();

    private:
        NodeClock clock;
        int nodeId;
        int ack_counter;
        bool leader;

        bool firstSend;
        bool startSimulation;

        int pulseTree;

        // NUM of trees must be equal to the number of SpanningTree and Neighbours

        SpanningTree spanning_trees[NUM_OF_TREES];
        Neighbours connected_neighbours[NUM_OF_TREES];

        // Initialize the list of capacity
        // Ensure that enough is buffered
        LinkCapacity linkCapacities[30];

        LeaderElection leader_election;
        Transaction transaction;

        FileWriter fileWriter;
        SaveState saveState;

        cMessage *event;    // pointer to the event object which will be used for timing
        cMessage *broadcast_tree; // variable to remember the message until its sent back

        virtual void sendMessagesFromBuffer(void);

        virtual std::string parseNodeID(const char* nodeName);

        // void run_simulation(void);
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
//    std::string total = saveState.loadstate(linkCapacities);
//    for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
//        connected_neighbours[i].add_capacities_to_linked_nodes(&linkCapacities[0], 23);
//    }
    // broadcastLeaderRequest();
//    for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
//        spanning_trees[i].wake_up();
//    }

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

//    fileWriter.set_node_id(nodeId, gateSize("out"));
//    fileWriter.initialize_file();

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

     transaction.set_connected_neighbours(connected_neighbours);
     transaction.set_node_id(nodeId);

     // Save state
     saveState.set_node_id_and_amount_of_nodes(nodeId, BasicNode::NUM_OF_NODES);
     saveState.set_neighbours(connected_neighbours, BasicNode::NUM_OF_TREES);


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
//        for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
//            spanning_trees[i].check_queued_messages();
//        }

        start_message_timer();

        // Finish Initialization process

//        if(simTime() > 1 and not startSimulation) {
//            for(int i = 0; i < BasicNode::NUM_OF_TREES; i++) {
//                spanning_trees[i].update_linked_nodes(connected_neighbours[i].get_linked_nodes());
//                connected_neighbours[i].add_capacities_to_linked_nodes(&linkCapacities[0], 23);
//            }
//            saveState.save();
//            startSimulation = true;
//            EV << "noideId: " << nodeId << "connectedneighbours string: " << connected_neighbours[0].to_string() << "\n";
//        }

//        if(simTime() > 205) {
            //int res = saveState.loadstate(linkCapacities);
            //EV << res;
//        }

        //
        // Start the random simulation here
        //


//        if(startSimulation and false) {
//            run_simulation();
//        }

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

//void BasicNode::run_simulation(void) {
//
//    int sendTransactionProbability = rand();
//
//    // 0.05 % chance of sending
//    if(sendTransactionProbability < 10000000) {
//        int amountProbability = rand() % 100;
//        int amount = 1;
//
//        if(amountProbability < 5) {
//            amount = 20;
//        } else if( amountProbability < 30) {
//            amount = 10;
//        } else if( amountProbability < 60) {
//            amount = 5;
//        }
//
//        int nodeProbability = rand() % 100;
//        int receiver = 0;
//
//        if(nodeProbability < 5) {
//            if(nodeProbability % 2 == 0) {
//                receiver = nodeId - 2;
//            } else {
//                receiver = nodeId + 2;
//            }
//        } else if(nodeProbability < 25) {
//            if(nodeProbability % 2 == 0) {
//                receiver = nodeId - 3;
//            } else {
//                receiver = nodeId + 3;
//            }
//        } else if(nodeProbability < 45) {
//            if(nodeProbability % 2 == 0) {
//                receiver = nodeId - 4;
//            } else {
//                receiver = nodeId + 4;
//            }
//        } else if(nodeProbability < 65) {
//            if(nodeProbability % 2 == 0) {
//                receiver = nodeId - 5;
//            } else {
//                receiver = nodeId + 5;
//            }
//        } else if(nodeProbability < 85) {
//            if(nodeProbability % 2 == 0) {
//                receiver = nodeId - 6;
//            } else {
//                receiver = nodeId + 6;
//            }
//        } else if(nodeProbability < 95) {
//            if(nodeProbability % 2 == 0) {
//                receiver = nodeId - 7;
//            } else {
//                receiver = nodeId + 7;
//            }
//        } else if(nodeProbability < 100) {
//            if(nodeProbability % 2 == 0) {
//                receiver = nodeId - 8;
//            } else {
//                receiver = nodeId + 8;
//            }
//        }
//
//        if(receiver < 0) {
//            receiver = nodeId - receiver;
//        }
//
//        if(receiver > 22) {
//            receiver = receiver - nodeId;
//        }
//
//        EV << "Node: " << nodeId << " \nSending Amount: " << amount << " \nReceiver: " << receiver << "\n";
//
//        int send = transaction.send(receiver, amount);
//
//    }
//
//    if(transaction.get_current_transaction_index() > 1) {
//
//            EV << "node: " << nodeId << " has transaction string with capacities: " << transaction.capacities_to_string() << " \n delay:" << transaction.delay_to_string() << "\n";
//    }
//
//    std::string time = simTime().str();
//
//    //Gather capacities on the lines
//    std::string capacities = "";
////    for(int i = 0; i < gateSize("out"); i++) {
////        capacities += linkCapacities[i].to_file();
////    }
//
//    fileWriter.update_variables(time,
//                                transaction.get_num_completed_transactions(),
//                                transaction.get_num_of_total_transactions(),
//                                transaction.get_num_forwarded_transactions(),
//                                transaction.get_num_forwarded_completed_transactions(),
//                                transaction.get_current_transaction_id(),
//                                transaction.get_failed_transactions(),
//                                transaction.get_capacity_failure(),
//                                transaction.get_network_delay(),
//                                transaction.get_crypto_delay(),
//                                transaction.get_os_delay(),
//                                capacities);
//
//}
