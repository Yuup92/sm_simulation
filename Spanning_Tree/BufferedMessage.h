//https://stackoverflow.com/questions/35464517/error-redefinition-of-class-but-no-redeclaration
#ifndef BUFFEREDMESSAGE_H__
#define BUFFEREDMESSAGE_H__

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class BufferedMessage
{
    public:
        BufferedMessage(void);
        ~BufferedMessage(void);
        BufferedMessage(cMessage*, int);
        cMessage * message;
        int out_gate_int;

        int getOutGateInt(void);
        cMessage * getMessage(void);

};

#endif
