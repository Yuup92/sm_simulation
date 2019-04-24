#ifndef MESSAGETYPE_H__
#define MESSAGETYPE_H__
//
//  MessageTypes:
//      1 - leaderMessage
//      2 - spanningTreeMessage
//
//


class MessageType
{

    public:
        static int get_leader_message_type(void);
        static int get_spanning_tree_message_type(void);

};

#endif
