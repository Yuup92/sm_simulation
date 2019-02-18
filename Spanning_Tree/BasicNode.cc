
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include <cstdlib>

using namespace omnetpp;

class BasicNode : public cSimpleModule
{
    private:
        int node_id;
        virtual std::string parseNodeID(const char* nodeName);

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
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

    EV << "Node " << getFullName() << " was set to: " << node_id << "\n";

    cMessage *msg = new cMessage("tictocMsg");
    for (int i = 0; i < 6; i++) {
        cMessage *copy = msg->dup();
        send(copy, "out", i);
    }
    delete msg;
}

void BasicNode::handleMessage(cMessage *msg)
{
    EV << "Received Message: " << msg->getArrivalGate()->getIndex();
    int arrival = msg->getArrivalGate()->getIndex();
    send(msg, "out", arrival); // send out the message
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
