
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
        virtual std::string parseNodeID(const char* nodeName);
        virtual BasicMessage* generateMessage();

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void broadcast();
};

Define_Module(BasicNode);

void BasicNode::initialize()
{
    if(true) {
        std::string id_string = this->parseNodeID(getFullName());
        node_id = std::stoi(id_string);
    } else {
        node_id = rand();
    }

    EV << "Node " << getFullName() << " has id: " << node_id << "\n";

    broadcast();
}

void BasicNode::handleMessage(cMessage *msg)
{
    // https://stackoverflow.com/questions/40873629/omnet-adding-functionalities-to-handlemessage-in-my-class
    BasicMessage * basicmsg = dynamic_cast<BasicMessage*> (msg);
    EV << "Received Message: " << basicmsg->getArrivalGate()->getIndex() << " with timestamp: " << basicmsg->getScalar_clock();
    int arrival = msg->getArrivalGate()->getIndex();


    BasicMessage * returnmsg = generateMessage();

    clock.increment_time();
    send(returnmsg, "out", arrival); // send out the message
}

void BasicNode::broadcast()
{

    for (int i = 0; i < 6; i++) {
        clock.increment_time();
        BasicMessage *msg = generateMessage();
        send(msg, "out", i);
    }

}

BasicMessage* BasicNode::generateMessage()
{
    int src = getIndex();
    int n = getVectorSize();

    char msgname[40];
    sprintf(msgname, "Message sent from node: %d", src);

    // Creating message
    BasicMessage *msg = new BasicMessage(msgname);
    msg->setSource(src);

    EV << "Scalar clock for src: " << src << " is currently: " << clock.get_scalar_time() << "\n";

    msg->setScalar_clock(clock.get_scalar_time());
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
