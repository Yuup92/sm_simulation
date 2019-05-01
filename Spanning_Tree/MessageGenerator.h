#ifndef MESSAGEGENERATOR_H__
#define MESSAGEGENERATOR_H__

#include <BufferedMessage.h>
#include <basicmsg_m.h>
#include <MessageType.h>
#include <SpanningTree.h>

// Tags for message type and who it needs to be refered to:
// Leader Election: LE
// Spanning Tree: ST%d, number is needed for sub spanning tree
//

// Static class used to generate the basic messages
class MessageGenerator
{
    public:
        MessageGenerator(){};

        // Leader messages:
        static BasicMessage * generateLeaderMessage(int);
        static BasicMessage * generateLeaderAckMessage(int);
        static BasicMessage * generateElectedLeaderMessage(int);

        // Spanning Tree:
        static BasicMessage * generateSpanningTreeRequest(int, int, int);
        static BasicMessage * generateStartSpanningTreeMessage(int, int);
        static BasicMessage * generateSpanningTreeAck(int, int);
        static BasicMessage * generateSpanningTreeDecline(int, int);
        static BasicMessage * generateSpanningTreeParentNotification(int, int, int);
        static BasicMessage * generateSpanningTreeChildNotification(int, int, int);

        static BasicMessage * generateSpanningTreePulse(int, int, int);
        static BasicMessage * generateSpanningTreeBroadCast(int, int);
        static BasicMessage * generateSpanningTreeBroadCastReply(int, int);

};

#endif
