#include <Neighbours.h>
#include <stdio.h>
#include <iostream>

Neighbours::Neighbours()
{
    num_of_neighbours = 0;
    num_of_neighbours_rand = 0;
}

Neighbours::Neighbours(int out_gate_size)
{
    num_of_neighbours = out_gate_size;

    for (int i = 0; i < out_gate_size; i++)
    {
        out_gate_list[i] = i;
    }

    int j = out_gate_size;

    // TODO
    // Implement this in a seperate function
    // Keeps removing the gate_index that has been added to the random list by shifting array points down
    for (int i = 0; i < out_gate_size; i++)
    {
        int r_index = rand() % j;
        random_gate_list[i] = out_gate_list[r_index];
        for(int k = r_index; k < j-1; k++)
        {
            out_gate_list[r_index] = out_gate_list[k + 1];
        }
        j--;
    }

}

// Maybe add a bool to reset out_gate_list or something
void Neighbours::set_number_of_neighbours(int out_gate_size)
{
    num_of_neighbours = out_gate_size;
    num_of_neighbours_rand = out_gate_size;
}

int Neighbours::get_amount_of_neighbours(void)
{
    return num_of_neighbours;
}

int * Neighbours::get_random_gate_list()
{
    return random_gate_list;
}

int * Neighbours::get_random_gates(int number_of_gates)
{

    for (int i = 0; i < num_of_neighbours_rand; i++)
    {
        gates[i] = random_gate_list[i];
    }

    if(num_of_neighbours_rand > 0) {
        for (int i = 0; i < num_of_neighbours_rand; i++)
        {
            int gate_0 = random_gate_list[0];

            for (int j = 0; j < num_of_neighbours_rand - 1; j++)
            {
                random_gate_list[j] = random_gate_list[j+1];
            }
            random_gate_list[num_of_neighbours_rand - 1] = gate_0;
        }
    }

    return gates;
}

void Neighbours::fill_array_with_random_gate_list(int *list) {
    for(int i = 0; i < num_of_neighbours_rand; i++) {
        *(list + i) = random_gate_list[i];
    }
}

void Neighbours::remove_neighbour_from_random_gate_list(int gateNumber) {

    int pos = -1;
    for(int i = 0; i < num_of_neighbours_rand; i++) {
        if(random_gate_list[i] == gateNumber) {
            pos = i;
        }
    }

    if (pos > -1) {
        for (int i = pos; i < num_of_neighbours_rand; ++i)
            random_gate_list[i] = random_gate_list[i + 1];
        num_of_neighbours_rand--;
    }

}

