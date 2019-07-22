#ifndef COMPUTERLATENCY_H_
#define COMPUTERLATENCY_H_

class ComputerLatency {

    public:
        ComputerLatency();
        double calculate_delay_ms();
        double get_delay();


    private:
        double delay;
        std::default_random_engine generator;

};


#endif
