#include <LeaderElection.h>

LeaderElection::LeaderElection(){};

void LeaderElection::setMsgBuf(MessageBuffer msg_buf)
{
    msg_buf = msg_buf;
}

MessageBuffer LeaderElection::getMessageBuffer(void)
{
    return msg_buf;
}

void LeaderElection::broadcastLeaderRequest()
{
    // https://stackoverflow.com/questions/3919850/conversion-from-myitem-to-non-scalar-type-myitem-requested
    for (int i = 0; i < 6; i++)
    {
        int node_id = 1;
        BasicMessage * msg = MessageGenerator::generateLeaderMessage(node_id,i);
        BufferedMessage * buf_msg = new BufferedMessage(msg, i);
        msg_buf.addMessage(buf_msg);
    }
}
