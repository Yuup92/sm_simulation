#include "src/latency/Latency.h"

Latency::Latency() {

}

double Latency::calculate_delay_ms(bool cryptography) {

    double totalDelay = 0.0;

    totalDelay += networkLatency.calculate_delay_ms();
    totalDelay += computerLatency.calculate_delay_ms();

    if(cryptography) {
        totalDelay += crytographicLatency.calculate_delay_ms();
    }


    return totalDelay;

}
