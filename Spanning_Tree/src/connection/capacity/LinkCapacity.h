#ifndef LINKCAPACITY_H_
#define LINKCAPACITY_H_

#include "src/transaction/concurrency/Concurrency.h"

struct pending_payment {
    int transactionId;
    int amount;
    bool increaseLink;
};

class LinkCapacity {

    public:
        LinkCapacity();
        LinkCapacity(int);

        void set_connected_node_id(int);
        int get_connected_node_id(void);

        int get_current_capacity(void);
        void set_current_capacity(int);

        int get_current_virtual_capacity(void);

        bool add_pending_transaction_increase(int, int);
        bool add_pending_transaction_decrease(int, int);

        bool complete_transaction(int);

        void remove_pending_transaction_index(int);
        void remove_pending_transaction_transaction_id(int);

        bool update_increase(int);
        bool update_decrease(int);


    private:
        int connectedNodeId;
        int capacity;
        int virtualCapacity;

        int numOfPayments;
        int numOfPaymentsPending;
        pending_payment pendingPayments[20];
        Concurrency concurrency[20];

};

#endif
