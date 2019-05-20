#include "src/transaction/Transaction.h"

Transaction::Transaction(){
    msgDelay = 0;
    concurrencyBlocking = new ConcurrencyBlocking();
    currentTransactionId = -1;
}

void Transaction::set_node_id(int id) {
    nodeId = id;
}


// TODO add this type of functionality
void Transaction::set_concurrency_type(int concurrency) {
    concurrencyType = concurrency;
}

void Transaction::set_connected_neighbours(Neighbours *n) {
    connectedNeighbours = n;
}

int Transaction::get_current_transaction_id(void) {
    return currentTransactionId;
}


void Transaction::handle_message(BasicMessage *msg, int outgoingEdge) {
    if(msg->getSubType() == Transaction::TRANSACTION_QUERY) {
        handle_query_message(outgoingEdge,
                             msg->getTransactionId(),
                             msg->getEndNodeId(),
                             msg->getAmount());
    } else if(msg->getSubType() == Transaction::TRANSACTOIN_QUERY_ACCEPT) {
        handle_query_accept_message(msg->getTransactionId());
    } else if(msg->getSubType() == Transaction::TRANSACTION_PUSH) {
        handle_transaction_push(outgoingEdge, msg->getEndNodeId(), msg->getTransactionId());
    } else if(msg->getSubType() == Transaction::TRANSACTION_PUSH_ACCEPT) {
        handle_transaction_push_reply(msg->getTransactionId());
    } else if(msg->getSubType() == Transaction::CLOSE_TRANSACTION) {
        handle_close_link(outgoingEdge, msg->getEndNodeId(), msg->getTransactionId());
    } else if(msg->getSubType() == Transaction::ACCEPT_TRANSACTION_CLOSE){
        handle_close_link_reply(msg->getTransactionId());
    }

}

void Transaction::handle_query_message(int outgoingEdge, int transId, int nId, int amount) {
    if(nodeId == nId) {
        if(not concurrencyBlocking->is_blocked()) {
            concurrencyBlocking->update_concurrency(true, transId, nId, outgoingEdge);
            update_message_buf(Transaction::initial_reply_request(transId), outgoingEdge);
        }
        // handle next transaction msg
    } else {
        if(not concurrencyBlocking->is_blocked()) {
            senderDirectionEdge = outgoingEdge;
            forward_send(transId, nId, amount);
        } else {
            // reply path is blocked
        }

    }
}

void Transaction::handle_query_accept_message(int transId) {
    if(currentTransactionId == transId) {
        // still need to make sure all channels come back
        update_message_buf(Transaction::transfer_payment(concurrencyBlocking->get_node_id(), transId), currentPaymentDirection);
        return;
    }

    if(concurrencyBlocking->get_transaction_id() == transId) {
        update_message_buf(Transaction::initial_reply_request(transId), senderDirectionEdge);
    } else {
        // do something because this path has closed
    }
}

void Transaction::handle_transaction_push(int outgoingEdge, int endNodeId, int transId) {
    if(endNodeId == nodeId) {
        update_message_buf(Transaction::transfer_payment_reply(transId), outgoingEdge);
        return;
    }

    if(concurrencyBlocking->is_blocked() and
            concurrencyBlocking->get_transaction_id() == transId) {
        update_message_buf(Transaction::transfer_payment(concurrencyBlocking->get_node_id(), transId), concurrencyBlocking->get_outgoing_edge());
    } else {
        // handle case channel is closed
    }
}

void Transaction::handle_transaction_push_reply(int transId) {
    if(currentTransactionId == transId) {
        update_message_buf(Transaction::close_transaction(concurrencyBlocking->get_node_id(), transId), currentPaymentDirection);
        return;
    }

    if(concurrencyBlocking->get_transaction_id() == transId) {
        update_message_buf(Transaction::transfer_payment_reply(transId), senderDirectionEdge);
    }
}

void Transaction::handle_close_link(int outgoingEdge, int endNodeId, int transId) {
    if(endNodeId == nodeId) {
        update_message_buf(Transaction::confirm_transaction_close(transId), outgoingEdge);
        concurrencyBlocking->release();
        currentTransactionId = -1;
        // add balance
        // redistribute link

        return;
    }

    if(concurrencyBlocking->is_blocked() and
            concurrencyBlocking->get_transaction_id() == transId) {
        update_message_buf(Transaction::close_transaction(concurrencyBlocking->get_node_id(), transId), concurrencyBlocking->get_outgoing_edge());
    } else {
        // handle when things have gone to the shit
    }

}

void Transaction::handle_close_link_reply(int transId) {

    if(currentTransactionId == transId) {
        concurrencyBlocking->release();
    }

    if(concurrencyBlocking->get_transaction_id() == transId) {
        update_message_buf(Transaction::confirm_transaction_close(transId), senderDirectionEdge);
        concurrencyBlocking->release();
        currentTransactionId = -1;
    }

}

int Transaction::get_message_count(void) {
    return msgBuf.getMessageCount();
}

BufferedMessage * Transaction::get_message(void) {
    return msgBuf.getMessage();
}

int Transaction::forward_send(int transactionId, int endNode, int amount) {
    int outgoingEdge = connectedNeighbours->get_outgoing_edge_transaction(endNode, amount);

    if(outgoingEdge > -1) {
          concurrencyBlocking->update_concurrency(true, transactionId, endNode, outgoingEdge);
          pendingTransactionAmount = amount;
          update_message_buf(Transaction::initial_send_request(endNode, amount, transactionId), outgoingEdge);
    } else {
        // return false;
        // non-blocking
        // not enough line capacity
    }
    return outgoingEdge;
}

int Transaction::send(int endNode, int amount) {
    int outgoingEdge = connectedNeighbours->get_outgoing_edge_transaction(endNode, amount);

    if(outgoingEdge > -1) {
          currentTransactionId = rand();
          concurrencyBlocking->update_concurrency(true, currentTransactionId, endNode, outgoingEdge);
          pendingTransactionAmount = amount;
          currentPaymentDirection = outgoingEdge;
          update_message_buf(Transaction::initial_send_request(endNode, amount, currentTransactionId), outgoingEdge);
    } else {
        // return false;
        // non-blocking
        // not enough line capacity
    }
    return outgoingEdge;
}

// TODO add msgDelay to the bufferedmessage process
//  or add it later to the game somehow the way it works now
//  is a little shitty
void Transaction::update_message_buf(BasicMessage *msg, int outgoingEdge) {
    msgDelay = msgDelay + 0.5;
    BufferedMessage * bufMsg = new BufferedMessage(msg, outgoingEdge, msgDelay);
    msgBuf.addMessage(bufMsg);
}

BasicMessage * Transaction::initial_send_request(int endNode, int amount, int transactionId) {
    char msgname[40];
    sprintf(msgname, "Initial transaction message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(Transaction::MESSAGE_TYPE);
    msg->setSubType(Transaction::TRANSACTION_QUERY);

    msg->setEndNodeId(endNode);
    msg->setAmount(amount);
    msg->setTransactionId(transactionId);

    return msg;
}

BasicMessage * Transaction::initial_reply_request(int transactionId) {
    char msgname[40];
    sprintf(msgname, "Accept initial transaction request");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(Transaction::MESSAGE_TYPE);
    msg->setSubType(Transaction::TRANSACTOIN_QUERY_ACCEPT);

    msg->setTransactionId(transactionId);

    return msg;
}

BasicMessage * Transaction::transfer_payment(int endNode, int transactionId) {
    char msgname[40];
    sprintf(msgname, "Transfer Payment");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(Transaction::MESSAGE_TYPE);
    msg->setSubType(Transaction::TRANSACTION_PUSH);

    msg->setEndNodeId(endNode);
    msg->setTransactionId(transactionId);

    return msg;
}

BasicMessage * Transaction::transfer_payment_reply(int transactionId) {
    char msgname[40];
    sprintf(msgname, "Transfer Accept Payment");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(Transaction::MESSAGE_TYPE);
    msg->setSubType(Transaction::TRANSACTION_PUSH_ACCEPT);

    msg->setTransactionId(transactionId);

    return msg;
}

BasicMessage * Transaction::close_transaction(int endNode, int transactionId) {
    char msgname[40];
    sprintf(msgname, "Finalize Transaction");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(Transaction::MESSAGE_TYPE);
    msg->setSubType(Transaction::CLOSE_TRANSACTION);

    msg->setEndNodeId(endNode);
    msg->setTransactionId(transactionId);

    return msg;
}

BasicMessage * Transaction::confirm_transaction_close(int transactionId) {
    char msgname[40];
    sprintf(msgname, "Accept Finilization Transaction");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(Transaction::MESSAGE_TYPE);
    msg->setSubType(Transaction::ACCEPT_TRANSACTION_CLOSE);

    msg->setTransactionId(transactionId);

    return msg;
}
