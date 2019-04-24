#include <LeaderElection.h>

// Implement that each node tells nodes it knows of an elected leader
// reply back via node hoping

LeaderElection::LeaderElection(){
    leader = false;
    sentMsgs = 0;
};

void LeaderElection::setMsgBuf(MessageBuffer msg_buf)
{
    msg_buf = msg_buf;
}

void LeaderElection::setId(int id)
{
    nodeId = id;
}

void LeaderElection::setAmountNeighbours(int numNeighbours)
{
    amountNeighbours = numNeighbours;
}

bool LeaderElection::isLeader(void)
{
    return leader;
}

MessageBuffer LeaderElection::getMessageBuffer(void)
{
    return msg_buf;
}

int LeaderElection::getMessageCount(void)
{
    return msg_buf.getMessageCount();
}

BufferedMessage * LeaderElection::getMessage(void)
{
    return msg_buf.getMessage();
}


void LeaderElection::broadcastLeaderRequest(void)
{
    // https://stackoverflow.com/questions/3919850/conversion-from-myitem-to-non-scalar-type-myitem-requested
    for (int i = 0; i < amountNeighbours; i++)
    {
        BasicMessage * msg = MessageGenerator::generateLeaderMessage(nodeId);
        BufferedMessage * buf_msg = new BufferedMessage(msg, i);
        msg_buf.addMessage(buf_msg);
        sentMsgs++;
    }
}

void LeaderElection::broadcastElectedLeader(void)
{
    for (int i = 0; i < amountNeighbours; i++)
    {
        BasicMessage * msg = MessageGenerator::generateElectedLeaderMessage(nodeId);
        BufferedMessage * buf_msg = new BufferedMessage(msg, i);
        msg_buf.addMessage(buf_msg);
    }
}

void LeaderElection::handleMessage(BasicMessage* msg, int outgoing_gate_id)
{
    if (nodeId < msg->getSrc_node_id())
    {
        BasicMessage * msg = MessageGenerator::generateLeaderAckMessage(nodeId);
        BufferedMessage * buf_msg = new BufferedMessage(msg, outgoing_gate_id);
        msg_buf.addMessage(buf_msg);
        // sentMsgs--;
    } else if ((msg->getAck()) and (!leader)){
        sentMsgs--;
        if (sentMsgs < 1) {
            leader = true;
            broadcastElectedLeader();
        }
    }
}
