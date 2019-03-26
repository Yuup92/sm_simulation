#include <MessageGenerator.h>

BasicMessage * MessageGenerator::generateLeaderMessage(int src, int destination)
{
    int int_gate = destination;

    // clock.increment_time();

    // int src = getIndex();
    char msgname[40];
    sprintf(msgname, "Message sent from node: %d", src);

    // Creating message
    BasicMessage * message = new BasicMessage(msgname);
    message->setSource(src);

    // EV << "Scalar clock for src: " << src << " is currently: " << clock.get_scalar_time() << "\n";

    // msg->setScalar_clock(clock.get_scalar_time());
    message->setSrc_node_id(src);
    message->setAck(false);

    return message;
    // ack_counter++;

}
