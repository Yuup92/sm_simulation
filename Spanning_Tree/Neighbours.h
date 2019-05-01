#ifndef NEIGHBOURS_H__
#define NEIGHBOURS_H__

#include <stdio.h>

class Neighbours
{
    private:
        int num_of_neighbours;
        int num_of_neighbours_rand;

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
        void fill_array_with_random_gate_list(int*);
        int * get_random_gate_list();
        void remove_neighbour_from_random_gate_list(int);
};

#endif
