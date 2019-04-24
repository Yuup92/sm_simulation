#include <Neighbours.h>
#include <stdio.h>
#include <iostream>

Neighbours::Neighbours()
{
    num_of_neighbours = 0;
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

    for (int i = 0; i < number_of_gates; i++)
    {
        gates[i] = random_gate_list[i];
    }

    // TODO
    // Shift the used gates to the back of the list
    // Implement this in a seperate function
    for (int i = 0; i < number_of_gates; i++)
    {
        int gate_0 = random_gate_list[0];

        for (int j = 0; j < num_of_neighbours - 1; j++)
        {
            random_gate_list[j] = random_gate_list[j+1];
        }
        random_gate_list[num_of_neighbours - 1] = gate_0;
    }

    return gates;
}

