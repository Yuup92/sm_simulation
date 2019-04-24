#include <MessageBuffer.h>
#include <BufferedMessage.h>
#include <MessageGenerator.h>

class LeaderElection
{
    public:
        LeaderElection();

        void setMsgBuf(MessageBuffer);
        void setId(int);
        void setAmountNeighbours(int);

        bool isLeader(void);

        void broadcastLeaderRequest(void);
        void broadcastElectedLeader(void);
        void handleMessage(BasicMessage*, int);

        int getMessageCount(void);
        BufferedMessage * getMessage(void);
        MessageBuffer getMessageBuffer(void);

    private:
        bool leader;

        MessageBuffer msg_buf;
        int nodeId;
        int amountNeighbours;
        int sentMsgs;

};
