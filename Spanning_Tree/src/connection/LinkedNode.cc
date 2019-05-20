#include "src/connection/LinkedNode.h"

LinkedNode::LinkedNode(){
    capacity = 100;
}

void LinkedNode::update_node(int outgoingEdge, int s, int w, int numChild, int *childList, bool _edgeTowardsRoot) {
    connectingEdge = outgoingEdge;
    state = s;
    weight = w;
    numOfChildren = numChild;
    for(int i = 0; i < numChild; i++) {
        children[i] = *(childList + i);
    }
    edgeTowardsRoot = _edgeTowardsRoot;

}

void LinkedNode::update_node(int outgoingEdge, int s, int w, int numChild, int *childList, bool _edgeTowardsRoot, int c) {
    connectingEdge = outgoingEdge;
    state = s;
    weight = w;
    numOfChildren = numChild;
    for(int i = 0; i < numChild; i++) {
        children[i] = *(childList + i);
    }
    capacity = c;
    edgeTowardsRoot = _edgeTowardsRoot;
}

void LinkedNode::set_connecting_edge(int outgoingEdge) {
    connectingEdge = outgoingEdge;
}

int LinkedNode::get_connecting_edge(void) {
    return connectingEdge;
}

void LinkedNode::set_state(int s) {
    state = s;
}

int LinkedNode::get_state(void) {
    return state;
}

void LinkedNode::set_weight(int w) {
    weight = w;
}

int LinkedNode::get_weight(void) {
    return weight;
}

void LinkedNode::set_number_of_children(int numChild) {
    numOfChildren = numChild;
}

int LinkedNode::get_number_of_children(void) {
    return numOfChildren;
}

void LinkedNode::set_children(int *childList) {
    for(int i = 0; i < numOfChildren; i++) {
        children[i] = *(childList + i);
    }
}

int * LinkedNode::get_children(void) {
    return &(children[0]);
}

int LinkedNode::get_child(int i) {
    return children[i];
}

void LinkedNode::set_capacity(int c) {
    capacity = c;
}

int LinkedNode::get_capacity(void) {
    return capacity;
}

int LinkedNode::process_transaction(int transactionAmount) {
    if((capacity - transactionAmount) > 0){
        capacity = capacity - transactionAmount;
        return capacity;
    } else {
        return -1;
    }
}

bool LinkedNode::get_edge_towards_root(void) {
    return edgeTowardsRoot;
}

void LinkedNode::set_edge_towards_root(bool edge) {
    edgeTowardsRoot = edge;
}

std::string LinkedNode::to_string(void) {

    std::string res = "";

    char buff[300];
    sprintf(buff, "outgoingEdge: %d \n numberOfChildren: %d \n capacity: %d \n", connectingEdge, numOfChildren, capacity);
    res = res + buff;

    char buf[300];
    res += "children id's: ";
    for(int i = 0; i < numOfChildren; i++) {
        res += std::to_string(children[i]);
        res += " , ";
    }
    res += "\n";

    if(edgeTowardsRoot) {
        res += " edge towards root \n";
    } else {
        res += " edge not towards root \n";
    }

    return res;

}
