
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
        // if (strcmp("node0", getName()) == 0) {
            // create and send first message on gate "out". "tictocMsg" is an
            // arbitrary string which will be the name of the message object.
            cMessage *msg = new cMessage("tictocMsg");

            for (int i = 0; i < 6; i++) {
                cMessage *copy = msg->dup();
                send(copy, "out", i);
            }
            delete msg;
        //}
    }

void BasicNode::handleMessage(cMessage *msg)
{
    EV << "Received Message: " << msg->getArrivalGate()->getIndex();
    int arrival = msg->getArrivalGate()->getIndex();
    send(msg, "out", arrival); // send out the message
}
