#ifndef MESSAGEGENERATOR_H__
#define MESSAGEGENERATOR_H__

#include <BufferedMessage.h>
#include <basicmsg_m.h>

// Tags for message type and who it needs to be refered to:
// Leader Election: LE
// Spanning Tree: ST%d, number is needed for sub spanning tree
// ??

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

#endif
