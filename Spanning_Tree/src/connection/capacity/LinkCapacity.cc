#include "src/connection/capacity/LinkCapacity.h"

LinkCapacity::LinkCapacity() {
    numOfPayments = 0;
    numOfPaymentsPending = 0;
}

LinkCapacity::LinkCapacity(int cap) {
    capacity = cap;
    virtualCapacity = cap;
    numOfPayments = 0;
    numOfPaymentsPending = 0;
}

void LinkCapacity::set_connected_node_id(int nodeId) {
    connectedNodeId = nodeId;
}

int LinkCapacity::get_connected_node_id(void) {
    return connectedNodeId;
}

void LinkCapacity::set_current_capacity(int cap) {
    capacity = cap;
    virtualCapacity = cap;
}

int LinkCapacity::get_current_capacity(void) {
    return capacity;
}

int LinkCapacity::get_current_virtual_capacity(void) {
    return virtualCapacity;
}

// Because a link cannot go  below 0, if capacity is added to the line
// it will not be added to the virtualcapacity because the sequence of the
// transactions has the potential of killing the line and may lead to payments
// thinking they can procceed while the line is down
bool LinkCapacity::add_pending_transaction_increase(int amount, int transactionId) {

    pendingPayments[numOfPaymentsPending].amount = amount;
    pendingPayments[numOfPaymentsPending].transactionId = transactionId;
    pendingPayments[numOfPaymentsPending].increaseLink = true;
    numOfPaymentsPending++;
    return true;

}

bool LinkCapacity::add_pending_transaction_decrease(int amount, int transactionId) {
    int newVirtualCapacity = virtualCapacity - amount;

    if(newVirtualCapacity > 0) {
        virtualCapacity = newVirtualCapacity;
        pendingPayments[numOfPaymentsPending].amount = amount;
        pendingPayments[numOfPaymentsPending].transactionId = transactionId;
        pendingPayments[numOfPaymentsPending].increaseLink = false;
        numOfPaymentsPending++;
        return true;
    } else {
        return false;
    }

}

bool LinkCapacity::complete_transaction(int transId) {
    int numOfPendingPayments = numOfPaymentsPending;

    for(int i = 0; i < numOfPendingPayments; i++) {
        if(pendingPayments[i].transactionId == transId) {
            if(pendingPayments[i].increaseLink == true) {
                capacity = capacity + pendingPayments[i].amount;
                virtualCapacity = virtualCapacity + pendingPayments[i].amount;
            } else {
                capacity = capacity - pendingPayments[i].amount;
            }
            numOfPayments++;
            return true;
        }
    }
    return false;
}

void LinkCapacity::remove_pending_transaction_index(int index) {
    if(numOfPaymentsPending == 1) {
        numOfPaymentsPending = 0;
        return;
    } else if(index == numOfPaymentsPending) {
        numOfPaymentsPending--;
    } else {
        for(int i = index; i < numOfPaymentsPending; i++) {
            if( (i+1) < numOfPaymentsPending) {
                pendingPayments[i] = pendingPayments[i + 1];
            }

        }
        numOfPaymentsPending--;
    }
}

void LinkCapacity::remove_pending_transaction_transaction_id(int transId) {

}

bool LinkCapacity::update_increase(int amount) {
    int cap = capacity + amount;

    if(cap > -1) {
        capacity = cap;
        return true;
    } else {
        return false;
    }
}

bool LinkCapacity::update_decrease(int amount) {
    int cap = capacity - amount;

    if(cap > -1) {
        capacity = cap;
        return true;
    } else {
        return false;
    }
}
