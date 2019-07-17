#ifndef TRANSACTION_H_
#define TRANSACTION_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "src/msg/outgoing_buf/MessageBuffer.h"
#include "src/msg/outgoing_buf/BufferedMessage.h"
#include "src/connection/Neighbours.h"
#include "src/transaction/concurrency/ConcurrencyBlocking.h"
#include "src/connection/LinkedNode.h"

// Struct that keeps track of which LinkedNodes currently a connection has been established with
struct transaction_connection{
    int amount;
    int transId;
    int indexLinkedNode;
    int edgeTowardsReceiver;
    int edgeTowardsSender;
    int endNode;
    int state;
    int neighbourhood;

    LinkedNode *linkTowardsReceiver;
    LinkedNode *linkTowardsSender;
};

class Transaction {

    public:
        Transaction();

        // PARAM
        static const int CHANNELS_USED = 10;

        // Message Types
        static const int MESSAGE_TYPE = 3;

        static const int CONCURRENCY_BLOCKING = 1;
        static const int CONCURRENCY_NON_BLOCKING = 2;

        static const int TRANSACTION_QUERY = 100;
        static const int TRANSACTOIN_QUERY_ACCEPT = 101;
        static const int TRANSACTION_REJECT = 110;

        static const int TRANSACTION_PUSH = 200;
        static const int TRANSACTION_PUSH_ACCEPT = 201;
        static const int TRANSACTION_PUSH_REJECT = 210;

        static const int CLOSE_TRANSACTION = 300;
        static const int ACCEPT_TRANSACTION_CLOSE = 305;

        static const int CAPACITY_ERROR = 600;
        static const int TIMEOUT = 605;
        static const int ERROR = 610;

        // States
        static const int STATE_WAITING = 100;

        static const int STATE_SENDING_NODE = 200;
        static const int STATE_FORWARDING_NODE = 210;
        static const int STATE_RECEIVING_NODE = 220;


        void set_node_id(int);
        void set_concurrency_type(int);
        void set_connected_neighbours(Neighbours *);

        int get_current_transaction_id(void);
        int get_current_transaction_index(void);

        // Message handling
        void handle_message(BasicMessage *, int);
        int get_message_count(void);
        BufferedMessage * get_message(void);

        int send(int, int);
        int forward_send(int, int, int, int, int);

        std::string to_string(void);

    private:

        int nodeId;
        int concurrencyType;
        int currentTransactionId;
        int pendingTransactionAmount;
        int senderDirectionEdge;
        int currentPaymentDirection;

        int currentNeighbour;
        int currentNeighbourList[10];
        Neighbours *connectedNeighbours;

        int numberOfCapacityErrors;
        int numberOfReceivedCapacityErrors;

        int numberOfTimeoutErrors;
        int numberOfReceivedTimeoutErrors;

        int numberOfErrors;
        int numberOfReceivedErrors;

        int currentState;

        MessageBuffer msgBuf;
        int msgDelay;

        int transactionConnectionIndex;
        transaction_connection transactionConnections[30];

        void handle_query_message(int, int, int, int, int);
        void handle_query_accept_message(int);

        void handle_transaction_push(int, int);
        void handle_transaction_push_reply(int);

        void handle_close_transaction(int, int, int);
        void handle_accept_close_transaction(int);

        void handle_close_link(int, int);
        void handle_close_link_reply(int);

        void handle_capacity_error(int);
        void handle_timeout_error(int);
        void handle_general_error(int);

        void update_message_buf(BasicMessage *, int);

        int get_trans_conn_index(int);
        void remove_transaction(int);

        static BasicMessage * initial_send_request(int, int, int, int);
        static BasicMessage * initial_reply_request(int);

        static BasicMessage * transfer_payment(int);
        static BasicMessage * transfer_payment_reply(int);

        static BasicMessage * close_transaction(int);
        static BasicMessage * confirm_transaction_close(int);

        static BasicMessage * capacity_error(int);
        static BasicMessage * timeout_error(int);
        static BasicMessage * general_error(int);

        static BasicMessage * transaction_failed(int, int, int, int);

};

#endif

