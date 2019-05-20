#ifndef LINKEDNODE_H_
#define LINKEDNODE_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <cstdlib>


class LinkedNode {
    public:
        LinkedNode();
        void update_node(int, int, int, int, int *, bool);
        void update_node(int, int, int, int, int *, bool, int);

        void set_connecting_edge(int);
        int get_connecting_edge(void);

        void set_state(int);
        int get_state(void);

        void set_weight(int);
        int get_weight(void);

        void set_number_of_children(int);
        int get_number_of_children(void);

        void set_children(int*);
        int * get_children(void);

        int get_child(int);

        void set_capacity(int);
        int get_capacity(void);
        int process_transaction(int);

        bool get_edge_towards_root(void);
        void set_edge_towards_root(bool);

        std::string to_string(void);

    private:
        int connectingEdge;
        int state;
        int weight;
        int numOfChildren;
        int children[20];
        int capacity;
        bool edgeTowardsRoot;
};

#endif

