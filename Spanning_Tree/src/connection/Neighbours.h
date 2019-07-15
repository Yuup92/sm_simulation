#ifndef NEIGHBOURS_H__
#define NEIGHBOURS_H__

#include <src/connection/LinkedNode.h>
#include <stdio.h>
#include <string.h>


class Neighbours
{
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

        void update_linked_nodes(int, int, int, int, int, int *, bool);
        LinkedNode * get_linked_nodes(void);
        LinkedNode * get_upstream_linked_node(int, int);
        LinkedNode * get_downstream_linked_node(int);

        void set_linked_node_updates(bool);
        bool is_linked_nodes_updated(void);

        void set_num_linked_nodes(int);
        int get_num_linked_nodes(void);

        int get_index_linked_nodes(int);
        int get_outgoing_edge_transaction(int, int);

        bool check_capacity(int, int);
        bool remove_capacity(int, int);
        bool add_capacity(int, int);

        std::string to_string(void);

    private:
        int num_of_neighbours;
        int num_of_neighbours_rand;

        int out_gate_list[10];
        int random_gate_list[10];

        int gates[10];
        bool linkedNodesUpdated;
        int numLinkedNodes;
        LinkedNode linkedNodes[20];
};

#endif
