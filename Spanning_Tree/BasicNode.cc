
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class BasicNode : public cSimpleModule
{
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(BasicNode);

void BasicNode::initialize()
{
    if (getIndex() == 0) {
        char msgname[20];
        sprintf(msgname, "tic-%d", getIndex());
        cMessage *msg = new cMessage(msgname);
        scheduleAt(0.0, msg);
    }
}

void BasicNode::handleMessage(cMessage *msg)
{
    EV << "Received message `" << msg->getName() << "', sending it out again\n";
    send(msg, "out");
}
