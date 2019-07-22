#include "src/latency/types/NetworkLatency.h"

NetworkLatency::NetworkLatency(void){

}

double NetworkLatency::calculate_delay_ms(void) {

    std::lognormal_distribution<double> distribution(0.002, 0.7);
    return distribution(generator) / 10.0;


}
