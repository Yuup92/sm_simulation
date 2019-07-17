#include "src/connection/Neighbours.h"
#include <stdio.h>
#include <iostream>

Neighbours::Neighbours()
{
    num_of_neighbours = 0;
    num_of_neighbours_rand = 0;
    linkedNodesUpdated = false;
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

void Neighbours::update_linked_nodes(int index, int outgoingEdge, int state, int weight, int numChild, int *childList, bool edgeTowardsRoot) {
    linkedNodes[index].update_node(outgoingEdge, state, weight, numChild, childList, edgeTowardsRoot);
}

LinkedNode * Neighbours::get_linked_nodes() {
    return &linkedNodes[0];
}

void Neighbours::set_linked_node_updates(bool updated) {
    linkedNodesUpdated = updated;
}

bool Neighbours::is_linked_nodes_updated(void) {
    return linkedNodesUpdated;
}

void Neighbours::set_num_linked_nodes(int amount) {
    numLinkedNodes = amount;
}

int Neighbours::get_num_linked_nodes(void) {
    return numLinkedNodes;
}

int Neighbours::get_index_linked_nodes(int edge) {

    for(int i = 0; i < numLinkedNodes; i++) {
        if(linkedNodes[i].get_connecting_edge() == edge) {
            return i;
        }
    }

}

int Neighbours::get_outgoing_edge_transaction(int nodeId, int amount) {
    int indexParent;
    int edgeTowardsTransaction = -1;
    bool edgeFound = false;

    for(int i = 0; i < numLinkedNodes; i++) {
        int *ptr = linkedNodes[i].get_children();
        for(int j = 0; j < linkedNodes[i].get_number_of_children(); j++) {
            if(*(ptr + j) == nodeId) {
                edgeFound = true;
                if(check_capacity(i, amount)) {
                    edgeTowardsTransaction = linkedNodes[i].get_connecting_edge();
                      return edgeTowardsTransaction;
                  }
            }
        }
    }

    return edgeTowardsTransaction;
}

// TODO
// if amount is not possible do something
LinkedNode * Neighbours::get_upstream_linked_node(int nodeId, int amount) {
    int indexParent;
    LinkedNode *node =  new LinkedNode();
    int edgeTowardsTransaction = 0;
    bool edgeFound = false;

    for(int i = 0; i < numLinkedNodes; i++) {
        int *ptr = linkedNodes[i].get_children();
        for(int j = 0; j < linkedNodes[i].get_number_of_children(); j++) {
            if(*(ptr + j) == nodeId) {
                return &linkedNodes[i];
            }
        }
    }

    for(int i = 0; i < numLinkedNodes; i++) {
        if(linkedNodes[i].get_edge_towards_root()) {
            return &linkedNodes[i];
        }
    }


    // return node;
}

// TODO build in safety
LinkedNode * Neighbours::get_downstream_linked_node(int outgoingEdge) {

    for(int i = 0; i < numLinkedNodes; i++) {
        if(linkedNodes[i].get_connecting_edge() == outgoingEdge) {
            return &linkedNodes[i];
        }
    }

}

bool Neighbours::check_capacity(int nodeId, int amount) {

    return true;

    for(int i = 0; i < numLinkedNodes; i++) {
        int *ptr = linkedNodes[i].get_children();
        for(int j = 0; j < linkedNodes[i].get_number_of_children(); j++) {
            if(*(ptr + j) == nodeId) {
                if(linkedNodes[i].get_capacity() > amount) {
                    return true;
                }
            }
        }
    }

    return false;

}

bool Neighbours::remove_capacity(int indexLinkedNodes, int amount) {

    if(check_capacity(indexLinkedNodes, amount)) {
        int cap = linkedNodes[indexLinkedNodes].get_capacity();
        linkedNodes[indexLinkedNodes].set_capacity(cap - amount);
        return true;
    } else {
        return false;
    }
}

bool Neighbours::add_capacity(int indexLinkedNodes, int amount) {

    if(check_capacity(indexLinkedNodes, amount)) {
        int cap = linkedNodes[indexLinkedNodes].get_capacity();
        linkedNodes[indexLinkedNodes].set_capacity(cap + amount);
        return true;
    } else {
        return false;
    }
}

std::string Neighbours::to_string(void) {
    std::string res = "";

    res += "Number of linked nodes: " + std::to_string(numLinkedNodes) + "\n";

    for(int i = 0; i < numLinkedNodes; i++) {
        res += linkedNodes[i].to_string();
    }

    return res;
}











