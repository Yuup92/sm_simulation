
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
        // Initialize is called at the beginning of the simulation.
        // To bootstrap the tic-toc-tic-toc process, one of the modules needs
        // to send the first message. Let this be `tic'.

        // Am I Tic or Toc?
        if (strcmp("node0", getName()) == 0) {
            // create and send first message on gate "out". "tictocMsg" is an
            // arbitrary string which will be the name of the message object.
            cMessage *msg = new cMessage("tictocMsg");

            send(msg, "out", 0);

//            for (int i = 0; i < 7; i++) {
//                send(msg, "out", i);
//            }

        }
    }

void BasicNode::handleMessage(cMessage *msg)
{
    EV << "Received Message: " << msg->getArrivalGate()->getIndex();
    int arrival = msg->getArrivalGate()->getIndex();
    send(msg, "out", arrival); // send out the message
}
