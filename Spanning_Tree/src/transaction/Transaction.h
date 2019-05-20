#ifndef TRANSACTION_H_
#define TRANSACTION_H_

#include <stdlib.h>

#include "src/msg/outgoing_buf/MessageBuffer.h"
#include "src/msg/outgoing_buf/BufferedMessage.h"
#include "src/connection/Neighbours.h"
#include "src/transaction/concurrency/ConcurrencyBlocking.h"

class Transaction {

    public:
        Transaction();

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


        void set_node_id(int);
        void set_concurrency_type(int);
        void set_connected_neighbours(Neighbours*);

        int get_current_transaction_id(void);

        // Message handling
        void handle_message(BasicMessage *, int);
        int get_message_count(void);
        BufferedMessage * get_message(void);

        int send(int, int);
        int forward_send(int, int, int);

    private:
        int nodeId;
        int concurrencyType;
        int currentTransactionId;
        int pendingTransactionAmount;
        int senderDirectionEdge;
        int currentPaymentDirection;
        ConcurrencyBlocking *concurrencyBlocking;
        Neighbours *connectedNeighbours;

        MessageBuffer msgBuf;
        int msgDelay;

        void handle_query_message(int, int, int, int);
        void handle_query_accept_message(int);

        void handle_transaction_push(int, int, int);
        void handle_transaction_push_reply(int);

        void handle_close_transaction(int, int, int);
        void handle_accept_close_transaction(int);

        void handle_close_link(int, int, int);
        void handle_close_link_reply(int);

        void update_message_buf(BasicMessage *, int);

        static BasicMessage * initial_send_request(int, int, int);
        static BasicMessage * initial_reply_request(int);

        static BasicMessage * transfer_payment(int, int);
        static BasicMessage * transfer_payment_reply(int);

        static BasicMessage * close_transaction(int, int);
        static BasicMessage * confirm_transaction_close(int);

};

#endif

