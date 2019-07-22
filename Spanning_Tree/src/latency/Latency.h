#ifndef LATENCY_H_
#define LATENCY_H_

#include "src/latency/types/NetworkLatency.h"
#include "src/latency/types/ComputerLatency.h"
#include "src/latency/types/CryptographicLatency.h"


class Latency {

    public:
        Latency();

        double calculate_delay_ms(bool);

    private:
        int delay;

        NetworkLatency networkLatency;
        ComputerLatency computerLatency;
        CryptographicLatency cryptographicLatency;


};

#endif
