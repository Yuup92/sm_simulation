
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

        bool part_of_tree;
        int tree_level;
        int parent_node;
        int child_nodes[2];
        int num_of_child_nodes;

        Neighbours connected_neighbours;

        virtual std::string parseNodeID(const char* nodeName);

        virtual BasicMessage* generateLeaderMessage();
        virtual BasicMessage* generateAckMessage();
        virtual BasicMessage* generateStartSpanningTreeMessage();
        virtual BasicMessage* generateSpanningTreeRequest();
        virtual BasicMessage* generateSpanningTreeAck();

        virtual bool receivingAck();

    protected:
        virtual void initialize() override;
        virtual void initialize_parameters();

        virtual void handleMessage(cMessage *msg) override;

        virtual void broadcastLeaderRequest();
        virtual void broadcastStartSpanningTree();

        virtual void spanningTreeRequest();
        virtual void acceptSpanningTreeRequest(int arrival_gate);
        virtual void declineSpanningTreeRequest(int arrival_gate);
};

Define_Module(BasicNode);

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

    connected_neighbours = Neighbours(gateSize("out"));

    // Get node ID or generate randomly
    if(true) {
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
    for (int i = 0; i < 6; i++) {
        BasicMessage *msg = generateStartSpanningTreeMessage();
        send(msg, "out", i);
    }
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
}

void BasicNode::acceptSpanningTreeRequest(int arrival_gate)
{
    BasicMessage *msg = generateSpanningTreeAck();
    send(msg, "out", arrival_gate);
}

void BasicNode::declineSpanningTreeRequest(int arrival_gate)
{
    clock.increment_time();

    char msgname[40];
    sprintf(msgname, "Accept child");

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);

    msg->setSpanning_decline_request(true);

    send(msg, "out", arrival_gate);
}

void BasicNode::handleMessage(cMessage *msg)
{
    // https://stackoverflow.com/questions/40873629/omnet-adding-functionalities-to-handlemessage-in-my-class
    BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (msg);
    EV << "Received Message: " << basicmsg->getArrivalGate()->getIndex() << " with timestamp: " << basicmsg->getScalar_clock();

    int incoming_node_id = basicmsg->getSrc_node_id();
    int arrival = msg->getArrivalGate()->getIndex();
    bool isMsgAck = basicmsg->getAck();

    bool start_spanning_tree_search = basicmsg->getStart_spanning_tree();
    bool spanning_request = basicmsg->getSpanning_request();
    bool spanning_request_ack = basicmsg->getSpanning_request_ack();
    bool spanning_decline = basicmsg->getSpanning_decline_request();

    if (spanning_decline) {
        if (!part_of_tree) {
            spanningTreeRequest();
        }
    } else if (spanning_request_ack) {
        if (!part_of_tree) {
            part_of_tree = true;
            parent_node = arrival;
            tree_level = basicmsg->getSpanning_tree_level() + 1;
            EV << "Node: " << getFullName()<< " has joined the tree at level: " << tree_level;
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

    char msgname[20];
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

    char msgname[60];
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
