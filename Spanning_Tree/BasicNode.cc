
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include <cstdlib>
#include <basicmsg_m.h>
#include <NodeClock.h>

using namespace omnetpp;

class BasicNode : public cSimpleModule
{
    private:
        NodeClock clock;
        int node_id;
        int ack_counter;
        bool leader;

        virtual std::string parseNodeID(const char* nodeName);
        virtual BasicMessage* generateLeaderMessage();
        virtual BasicMessage* generateAckMessage();

        virtual bool receivingAck();

    protected:
        virtual void initialize() override;
        virtual void initialize_parameters();

        virtual void handleMessage(cMessage *msg) override;

        virtual void broadcastLeaderRequest();
        virtual void broadcastStartSpanningTree();
};

Define_Module(BasicNode);

void BasicNode::initialize()
{
    initialize_parameters();
    broadcastLeaderRequest();
}

void BasicNode::initialize_parameters()
{
    leader = false;
    ack_counter = 0;

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

void BasicNode::handleMessage(cMessage *msg)
{
    // https://stackoverflow.com/questions/40873629/omnet-adding-functionalities-to-handlemessage-in-my-class
    BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (msg);
    EV << "Received Message: " << basicmsg->getArrivalGate()->getIndex() << " with timestamp: " << basicmsg->getScalar_clock();

    int incoming_node_id = basicmsg->getSrc_node_id();
    int arrival = msg->getArrivalGate()->getIndex();
    bool isMsgAck = basicmsg->getAck();

    if ((!isMsgAck) && (node_id < incoming_node_id)) {
        BasicMessage * ack_msg = generateAckMessage();
        send(ack_msg, "out", arrival);
    } else if (isMsgAck) {
        if (receivingAck()) {

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
