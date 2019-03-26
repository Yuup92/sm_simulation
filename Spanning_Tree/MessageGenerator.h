#include <BufferedMessage.h>
#include <basicmsg_m.h>

// Static class used to generate the basic messages
class MessageGenerator
{
    public:
        MessageGenerator(){};

        // Leader messages:
        static BasicMessage * generateLeaderMessage(int, int);
        static BasicMessage * generateAckMessage(int);

        // Spanning Tree:
        static BasicMessage * generateStartSpanningTreeMessage(int);
        static BasicMessage * generateSpanningTreeAck(int, int);
        static BasicMessage * generateSpanningTreeDecline(int);
        static BasicMessage * generateSpanningTreeBroadCast(int);
        static BasicMessage * generateSpanningTreeBroadCastReply(int);
        static BasicMessage * generateSpanningTreeRequest(int);
};
