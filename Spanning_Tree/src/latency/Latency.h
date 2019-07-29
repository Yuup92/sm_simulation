#ifndef LATENCY_H_
#define LATENCY_H_

#include "src/latency/types/NetworkLatency.h"
#include "src/latency/types/ComputerLatency.h"
#include "src/latency/types/CryptographicLatency.h"


class Latency {

    public:
        Latency();

        double calculate_delay_ms(bool);

        double get_network_delay(void);
        double get_os_delay(void);
        double get_crypto_delay(void);

    private:
        int delay;

        NetworkLatency networkLatency;
        ComputerLatency computerLatency;
        CryptographicLatency cryptographicLatency;


};

#endif
