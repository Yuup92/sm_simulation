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

// SpanningTree

BasicMessage * MessageGenerator::generateStartSpanningTreeMessage(int node_id, int lowestFragmentId)
{
    char msgname[120];
    sprintf(msgname, "Leader has been elected: %d. Starting Spanning tree process", node_id);

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(MessageType::get_spanning_tree_message_type());

    msg->setSrc_node_id(node_id);
    msg->setLowestIdFragment(lowestFragmentId);
    msg->setAck(false);

    msg->setStart_spanning_tree(true);
    msg->setRoot_node(node_id);

    return msg;
}

BasicMessage * MessageGenerator::generateSpanningTreeRequest(int node_id, int lowestFragmentId, int rootNodeId)
{
    char msgname[80];
    sprintf(msgname, "%d: Requesting to join spanning tree", node_id);

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(MessageType::get_spanning_tree_message_type());
    msg->setSubType(SpanningTree::REQUEST);
    msg->setSrc_node_id(node_id);
    msg->setLowestIdFragment(lowestFragmentId);
    msg->setRootNodeId(rootNodeId);
    msg->setAck(false);

    msg->setSpanning_request(true);

    return msg;
}

BasicMessage * MessageGenerator::generateSpanningTreeAck(int node_id, int lowestFragmentId)
{

    char msgname[80];
    sprintf(msgname, "%d: Accept parent", node_id);

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(MessageType::get_spanning_tree_message_type());
    msg->setSubType(SpanningTree::ACCEPT_REQUEST);
    msg->setSrc_node_id(node_id);
    msg->setLowestIdFragment(lowestFragmentId);
    msg->setAck(false);

    msg->setSpanning_request_ack(true);

    return msg;
}

BasicMessage * MessageGenerator::generateSpanningTreeDecline(int node_id, int lowestFragmentId)
{
    char msgname[80];
    sprintf(msgname, "%d: Decline parent", node_id);

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(MessageType::get_spanning_tree_message_type());
    msg->setSubType(SpanningTree::DECLINE_REQUEST);
    msg->setSrc_node_id(node_id);
    msg->setLowestIdFragment(lowestFragmentId);
    msg->setAck(false);

    msg->setSpanning_decline_request(true);

    return msg;
}

BasicMessage * MessageGenerator::generateSpanningTreeParentNotification(int node_id, int lowestFragmentId, int rootNodeId){
    char msgname[80];
    sprintf(msgname, "%d: Parent Notifcation", node_id);

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(MessageType::get_spanning_tree_message_type());
    msg->setSubType(SpanningTree::NOTIFY_PARENT);
    msg->setSrc_node_id(node_id);
    msg->setRootNodeId(rootNodeId);
    msg->setLowestIdFragment(lowestFragmentId);

    return msg;
}

BasicMessage * MessageGenerator::generateSpanningTreeChildNotification(int node_id, int lowestFragmentId, int rootNodeId){
    char msgname[80];
    sprintf(msgname, "%d: Notify child", node_id);

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(MessageType::get_spanning_tree_message_type());
    msg->setSubType(SpanningTree::NOTIFY_CHILD);
    msg->setSrc_node_id(node_id);
    msg->setRootNodeId(rootNodeId);
    msg->setLowestIdFragment(lowestFragmentId);

    return msg;
}

BasicMessage * MessageGenerator::generateSpanningTreePulse(int pulse, int node_ID, int lowestFragmentId)
{
    char msgname[80];
    sprintf(msgname, "Pulse: %d, nodes ID: %d", pulse, node_ID);

    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(MessageType::get_spanning_tree_message_type());
    msg->setSubType(SpanningTree::PULSE);
    msg->setPulseNum(pulse);
    msg->setLowestIdFragment(lowestFragmentId);

    msg->setAck(false);

    msg->setSpanning_decline_request(true);

    return msg;
}


BasicMessage * MessageGenerator::generateSpanningTreeBroadCast(int node_id, int lowestFragmentId)
{
    char msgname[40];
    sprintf(msgname, "Broadcast request through tree");

    BasicMessage *msg = new BasicMessage(msgname);
    msg->setSubType(SpanningTree::PULSE);
    msg->setType(MessageType::get_spanning_tree_message_type());
    msg->setSrc_node_id(node_id);
    msg->setAck(false);
    msg->setLowestIdFragment(lowestFragmentId);

    msg->setDown_broadcast(true);

    return msg;
}

BasicMessage * MessageGenerator::generateSpanningTreeBroadCastReply(int node_id, int lowestFragmentId)
{
    char msgname[40];
    sprintf(msgname, "Reply to broadcast request");

    BasicMessage *msg = new BasicMessage(msgname);
    msg->setSubType(SpanningTree::UPLINK);
    msg->setType(MessageType::get_spanning_tree_message_type());
    msg->setSrc_node_id(node_id);
    msg->setLowestIdFragment(lowestFragmentId);
    msg->setAck(false);

    msg->setUp_broadcast_reply(true);

    return msg;
}
