#include <BufferedMessage.h>

BufferedMessage::BufferedMessage(void){};

BufferedMessage::BufferedMessage(cMessage * msg, int gate)
{
    message = msg;
    out_gate_int = gate;
}

int BufferedMessage::getOutGateInt(void)
{
    return out_gate_int;
}

cMessage * BufferedMessage::getMessage(void)
{
    return message;
}

