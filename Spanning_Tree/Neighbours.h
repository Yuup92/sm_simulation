#include <stdio.h>

class Neighbours
{
    private:
        int num_of_neighbours;
        int out_gate_list[10];
        int random_gate_list[10];

        int gates[10];

    public:
        Neighbours();
        Neighbours(int out_gate_size);

        void set_number_of_neighbours(int out_gate_size);
        // std::vector <int> get_out_gate_list();

        int get_amount_of_neighbours();
        int * get_random_gates(int number_of_gates);
        int * get_random_gate_list();
};
