#include <NodeClock.h>
#include <stdio.h>

NodeClock::NodeClock(void)
{
    scalar_time = 0;
}

void NodeClock::increment_time(void)
{
    scalar_time++;
}

int NodeClock::get_scalar_time(void)
{
    return scalar_time;
}
