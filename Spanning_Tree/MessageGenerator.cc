#include <MessageGenerator.h>



// Leader Election
BasicMessage * MessageGenerator::generateLeaderMessage(int src)
{
    char msgname[40];
    sprintf(msgname, "Message sent from node: %d", src);

    // Creating message
    BasicMessage * message = new BasicMessage(msgname);

    message->setType(MessageType::get_leader_message_type());
    message->setSource(src);
    message->setSrc_node_id(src);
    message->setAck(false);

    return message;
}

// Leader Election
BasicMessage * MessageGenerator::generateLeaderAckMessage(int src)
{
    // clock.increment_time();

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

// SpanningTree

BasicMessage * MessageGenerator::generateStartSpanningTreeMessage(int node_id)
{
    //clock.increment_time();

    char msgname[120];
    sprintf(msgname, "Leader has been elected: %d. Starting Spanning tree process", node_id);

    BasicMessage *msg = new BasicMessage(msgname);

    // msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);

    msg->setStart_spanning_tree(true);
    msg->setRoot_node(node_id);

    return msg;
}

BasicMessage * MessageGenerator::generateSpanningTreeRequest(int node_id)
{
    //clock.increment_time();

    char msgname[40];
    sprintf(msgname, "Requesting to join spanning tree");

    BasicMessage *msg = new BasicMessage(msgname);

    // msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);

    msg->setSpanning_request(true);

    return msg;
}

BasicMessage * MessageGenerator::generateSpanningTreeAck(int node_id, int tree_level)
{
    // clock.increment_time();

    char msgname[40];
    sprintf(msgname, "Accept child");

    BasicMessage *msg = new BasicMessage(msgname);

    // msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);

    msg->setSpanning_request_ack(true);
    msg->setSpanning_tree_level(tree_level);

    return msg;
}

BasicMessage * MessageGenerator::generateSpanningTreeDecline(int node_id)
{
    // clock.increment_time();

    char msgname[40];
    sprintf(msgname, "Decline child");

    BasicMessage *msg = new BasicMessage(msgname);

    // msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);

    msg->setSpanning_decline_request(true);

    return msg;
}

BasicMessage * MessageGenerator::generateSpanningTreeBroadCast(int node_id)
{
    // clock.increment_time();

    char msgname[40];
    sprintf(msgname, "Broadcast request through tree");

    BasicMessage *msg = new BasicMessage(msgname);

    // msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);

    msg->setDown_broadcast(true);

    return msg;
}

BasicMessage * MessageGenerator::generateSpanningTreeBroadCastReply(int node_id)
{
    // clock.increment_time();

    char msgname[40];
    sprintf(msgname, "Reply to broadcast request");

    BasicMessage *msg = new BasicMessage(msgname);

    // msg->setScalar_clock(clock.get_scalar_time());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);

    msg->setUp_broadcast_reply(true);

    return msg;
}
