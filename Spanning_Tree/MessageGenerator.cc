#include <MessageGenerator.h>



// Leader Election
BasicMessage * MessageGenerator::generateLeaderMessage(int src)
{
    char msgname[40];
    sprintf(msgname, "Message sent from node: %d", src);

    // Creating message
    BasicMessage * msg = new BasicMessage(msgname);

    msg->setType(MessageType::get_leader_message_type());
    msg->setSource(src);
    msg->setSrc_node_id(src);
    msg->setAck(false);

    return msg;
}

// Leader Election
BasicMessage * MessageGenerator::generateLeaderAckMessage(int src)
{
    char msgname[120];
    sprintf(msgname, "Ack from: %d", src);

    BasicMessage *msg = new BasicMessage(msgname);

    // msg->setScalar_clock(clock.get_scalar_time());
    msg->setType(MessageType::get_leader_message_type());
    msg->setSrc_node_id(src);
    msg->setAck(true);

    return msg;
}

BasicMessage * MessageGenerator::generateElectedLeaderMessage(int src)
{
    char msgname[120];
    sprintf(msgname, "Elected leader %d", src);

    BasicMessage *msg = new BasicMessage(msgname);
    msg->setType(MessageType::get_leader_message_type());
    msg->setSrc_node_id(src);
    msg->setAck(false);
    msg->setLeaderId(src);

    return msg;

}
