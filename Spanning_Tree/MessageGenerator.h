#include <BufferedMessage.h>
#include <basicmsg_m.h>

// Static class used to generate the basic messages
class MessageGenerator
{
    public:
        MessageGenerator(){};

        // Leader messages:
        static BasicMessage * generateLeaderMessage(int, int);

};
