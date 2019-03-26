#include <MessageBuffer.h>
#include <BufferedMessage.h>
#include <MessageGenerator.h>

class LeaderElection
{
    public:
        LeaderElection();

        void setMsgBuf(MessageBuffer);
        void broadcastLeaderRequest(void);
        MessageBuffer getMessageBuffer(void);

    private:
        MessageBuffer msg_buf;
};
