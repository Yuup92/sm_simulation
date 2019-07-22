#include "src/transaction/Transaction.h"

Transaction::Transaction(){
    msgDelay = 0;
    currentTransactionId = -1;
    currentState = Transaction::STATE_WAITING;
    transactionConnectionIndex = 0;

    numberOfCapacityErrors = 0;
    numberOfReceivedCapacityErrors = 0;

    numberOfTimeoutErrors = 0;
    numberOfReceivedTimeoutErrors = 0;

    numberOfErrors = 0;
    numberOfReceivedErrors = 0;

    currentNeighbour = 0;
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

int Transaction::get_current_transaction_index(void) {
    return transactionConnectionIndex;
}

int Transaction::send(int endNode, int amount) {
    LinkedNode *node = connectedNeighbours[transactionConnections[transactionConnectionIndex].neighbourhood].get_upstream_linked_node(endNode, amount);

    if(node->get_capacity() > amount) {
        transactionConnections[transactionConnectionIndex].amount = amount;
        transactionConnections[transactionConnectionIndex].endNode = endNode;
        transactionConnections[transactionConnectionIndex].transId = rand();
        transactionConnections[transactionConnectionIndex].state = Transaction::STATE_SENDING_NODE;
        transactionConnections[transactionConnectionIndex].neighbourhood = currentNeighbour;

        // Get linked Node
        transactionConnections[transactionConnectionIndex].linkTowardsReceiver = node;
        transactionConnections[transactionConnectionIndex].edgeTowardsReceiver = transactionConnections[transactionConnectionIndex].linkTowardsReceiver->get_connecting_edge();
        transactionConnections[transactionConnectionIndex].edgeTowardsSender = -1;

        update_message_buf(Transaction::initial_send_request(transactionConnections[transactionConnectionIndex].endNode,
                                                                transactionConnections[transactionConnectionIndex].amount,
                                                                transactionConnections[transactionConnectionIndex].transId,
                                                                transactionConnections[transactionConnectionIndex].neighbourhood),
                                                                transactionConnections[transactionConnectionIndex].edgeTowardsReceiver);
        // All parameters saved in the index
        transactionConnectionIndex++;
        return transactionConnections[transactionConnectionIndex].edgeTowardsReceiver;
    } else {
        return 0;
    }

}

void Transaction::handle_message(BasicMessage *msg, int outgoingEdge) {

    // No index for the transaction ID
    if(msg->getSubType() == Transaction::TRANSACTION_QUERY) {
            handle_query_message(outgoingEdge,
                                msg->getTransactionId(),
                                 msg->getEndNodeId(),
                                 msg->getAmount(),
                                 msg->getNeighbourhoodIndex());
    }

    int index = get_trans_conn_index(msg->getTransactionId());

    if(index < 0) {
        return;
    } else if(msg->getSubType() == Transaction::TRANSACTOIN_QUERY_ACCEPT) {
        handle_query_accept_message(index);
    } else if(msg->getSubType() == Transaction::TRANSACTION_PUSH) {
        handle_transaction_push(outgoingEdge, index);
    } else if(msg->getSubType() == Transaction::TRANSACTION_PUSH_ACCEPT) {
        handle_transaction_push_reply(index);
    } else if(msg->getSubType() == Transaction::CLOSE_TRANSACTION) {
        handle_close_link(outgoingEdge, index);
    } else if(msg->getSubType() == Transaction::ACCEPT_TRANSACTION_CLOSE){
        handle_close_link_reply(index);
    } else if(msg->getSubType() == Transaction::CAPACITY_ERROR) {
        handle_capacity_error(index);
    } else if(msg->getSubType() == Transaction::TIMEOUT) {
        handle_timeout_error(index);
    } else if(msg->getSubType() == Transaction::ERROR) {
        handle_general_error(index);
    }

}

void Transaction::handle_query_message(int outgoingEdge, int transId, int nId, int amount, int neighbourhood) {
    // Message has found receiver
    if(nodeId == nId) {
        transactionConnections[transactionConnectionIndex].amount = amount;
        transactionConnections[transactionConnectionIndex].endNode = nId;
        transactionConnections[transactionConnectionIndex].transId = transId;
        transactionConnections[transactionConnectionIndex].state = Transaction::STATE_RECEIVING_NODE;
        transactionConnections[transactionConnectionIndex].neighbourhood = neighbourhood;


        transactionConnections[transactionConnectionIndex].edgeTowardsReceiver = -1;
        transactionConnections[transactionConnectionIndex].edgeTowardsSender = outgoingEdge;

        transactionConnections[transactionConnectionIndex].linkTowardsSender = connectedNeighbours[transactionConnections[transactionConnectionIndex].neighbourhood].get_downstream_linked_node(outgoingEdge);

        update_message_buf(Transaction::initial_reply_request(transactionConnections[transactionConnectionIndex].transId),
                                                               transactionConnections[transactionConnectionIndex].edgeTowardsSender);
        transactionConnectionIndex++;
        // handle next transaction msg

    // Message needs to be forwarded
    } else {
        // TODO check if path has enough capacity to send futher
        // TODO check if transaction ID doesnt already exist
        forward_send(transId, nId, amount, outgoingEdge, neighbourhood);
    }
}


int Transaction::forward_send(int transactionId, int endNode, int amount, int senderEdge, int neighbourhood) {
    LinkedNode *node = connectedNeighbours[transactionConnections[transactionConnectionIndex].neighbourhood].get_upstream_linked_node(endNode, amount);

    if(node->get_capacity() > amount) {
        transactionConnections[transactionConnectionIndex].amount = amount;
        transactionConnections[transactionConnectionIndex].endNode = endNode;
        transactionConnections[transactionConnectionIndex].transId = transactionId;
        transactionConnections[transactionConnectionIndex].state = Transaction::STATE_FORWARDING_NODE;
        transactionConnections[transactionConnectionIndex].neighbourhood = neighbourhood;

        // Get linked Node
        transactionConnections[transactionConnectionIndex].linkTowardsReceiver = connectedNeighbours[transactionConnections[transactionConnectionIndex].neighbourhood].get_upstream_linked_node(endNode, amount);
        transactionConnections[transactionConnectionIndex].linkTowardsSender = connectedNeighbours[transactionConnections[transactionConnectionIndex].neighbourhood].get_downstream_linked_node(senderEdge);

        transactionConnections[transactionConnectionIndex].edgeTowardsReceiver = transactionConnections[transactionConnectionIndex].linkTowardsReceiver->get_connecting_edge();
        transactionConnections[transactionConnectionIndex].edgeTowardsSender = senderEdge;

        update_message_buf(Transaction::initial_send_request(transactionConnections[transactionConnectionIndex].endNode,
                                                             transactionConnections[transactionConnectionIndex].amount,
                                                             transactionConnections[transactionConnectionIndex].transId,
                                                             transactionConnections[transactionConnectionIndex].neighbourhood),
                                                             transactionConnections[transactionConnectionIndex].edgeTowardsReceiver);
        // All parameters saved in the index
        transactionConnectionIndex++;
        return 1;
    } else {
        update_message_buf(Transaction::capacity_error(transactionId), senderEdge);
    }
    return -1;
}

void Transaction::handle_query_accept_message(int index) {
    // Current Node started transaction
    if(transactionConnections[index].state == Transaction::STATE_SENDING_NODE) {

        transactionConnections[index].linkTowardsReceiver->pend_decrease_transaction(transactionConnections[index].amount,
                                                                                        transactionConnections[index].transId);


        update_message_buf(Transaction::transfer_payment(transactionConnections[index].transId),
                                                            transactionConnections[index].edgeTowardsReceiver);
        return;
    // Current Node is forwarding transaction
    } else if(transactionConnections[index].state == Transaction::STATE_FORWARDING_NODE){

        update_message_buf(Transaction::initial_reply_request(transactionConnections[index].transId),
                                                                transactionConnections[index].edgeTowardsSender);
        return;
    }
}

void Transaction::handle_transaction_push(int outgoingEdge, int index) {

    if(transactionConnections[index].state == Transaction::STATE_RECEIVING_NODE) {
        bool capacityAcquired = transactionConnections[index].linkTowardsSender->pend_increase_transaction(transactionConnections[index].amount,
                                                                                    transactionConnections[index].transId);

        if(capacityAcquired == true) {
            update_message_buf(Transaction::transfer_payment_reply(transactionConnections[index].transId),
                                                                    transactionConnections[index].edgeTowardsSender );
            return;
        } else {
            update_message_buf(Transaction::capacity_error(transactionConnections[index].transId), transactionConnections[index].edgeTowardsSender);
        }
    } else if(transactionConnections[index].state == Transaction::STATE_FORWARDING_NODE) {


        bool capacityAcquiredUpstream = transactionConnections[index].linkTowardsSender->pend_increase_transaction(transactionConnections[index].amount,
                                                                                    transactionConnections[index].transId);

        bool capacityAcquiredDownstream = transactionConnections[index].linkTowardsReceiver->pend_decrease_transaction(transactionConnections[index].amount,
                                                                                    transactionConnections[index].transId);

        if(capacityAcquiredUpstream == true and
                capacityAcquiredDownstream == true) {
            update_message_buf(Transaction::transfer_payment(transactionConnections[index].transId),
                                                                transactionConnections[index].edgeTowardsReceiver);
        } else {
            update_message_buf(Transaction::capacity_error(transactionConnections[index].transId), transactionConnections[index].edgeTowardsSender);
        }

        return;
    } else {
        // Something went wrong send a general error, rest of the transactions should timeout.
        update_message_buf(Transaction::general_error(transactionConnections[index].transId), transactionConnections[index].edgeTowardsSender);
    }
}

void Transaction::handle_transaction_push_reply(int index) {

    if(transactionConnections[index].state == Transaction::STATE_SENDING_NODE) {
        update_message_buf(Transaction::close_transaction(transactionConnections[index].transId),
                                                            transactionConnections[index].edgeTowardsReceiver);
        return;
    } else if(transactionConnections[index].state == Transaction::STATE_FORWARDING_NODE) {
        update_message_buf(Transaction::transfer_payment_reply(transactionConnections[index].transId),
                transactionConnections[index].edgeTowardsSender);
    }
}

void Transaction::handle_close_link(int outgoingEdge, int index) {

    if(transactionConnections[index].state == Transaction::STATE_RECEIVING_NODE) {
        update_message_buf(Transaction::confirm_transaction_close(transactionConnections[index].transId),
                                                                    transactionConnections[index].edgeTowardsSender);

        transactionConnections[index].linkTowardsSender->update_capacity(transactionConnections[index].transId);
        remove_transaction(index);
        return;
    } else if(transactionConnections[index].state == Transaction::STATE_FORWARDING_NODE) {
        update_message_buf(Transaction::close_transaction(transactionConnections[index].transId),
                                                            transactionConnections[index].edgeTowardsReceiver);
        return;
    } else {
        // Something went wrong send a general error, rest of the transactions should timeout.
        update_message_buf(Transaction::general_error(transactionConnections[index].transId), transactionConnections[index].edgeTowardsSender);
        return;
    }
}

void Transaction::handle_close_link_reply(int index) {
    // TODO most work needs to be done here

    if(transactionConnections[index].state == Transaction::STATE_SENDING_NODE) {

        bool decreaseUpdate = transactionConnections[index].linkTowardsReceiver->update_capacity(transactionConnections[index].transId);
        remove_transaction(index);
        return;
    } else if(transactionConnections[index].state == Transaction::STATE_FORWARDING_NODE) {
        //Update Capacities
        bool increaseUpdate = transactionConnections[index].linkTowardsSender->update_capacity(transactionConnections[index].transId);
        bool decreaseUpdate = transactionConnections[index].linkTowardsReceiver->update_capacity(transactionConnections[index].transId);

        if(increaseUpdate && decreaseUpdate) {
            update_message_buf(Transaction::confirm_transaction_close(transactionConnections[index].transId),
                                                                        transactionConnections[index].edgeTowardsSender);
            remove_transaction(index);
        } else {
            //TODO
            // figure out which one failed and make sure whole transaction is reset
        }

    }
}

int Transaction::get_message_count(void) {
    return msgBuf.getMessageCount();
}

BufferedMessage * Transaction::get_message(void) {
    return msgBuf.getMessage();
}



void Transaction::handle_capacity_error(int index) {

    if(transactionConnections[index].state == Transaction::STATE_SENDING_NODE) {
        remove_transaction(index);
        numberOfReceivedCapacityErrors++;

        if(currentNeighbour < 9) {
            currentNeighbour++;
        } else {
            currentNeighbour = 0;
        }

        // Remove the transaction from the list
        // Or try a different path
    } else if(transactionConnections[index].state == Transaction::STATE_FORWARDING_NODE) {
        update_message_buf(Transaction::capacity_error(transactionConnections[index].transId),
                                                                 transactionConnections[index].edgeTowardsSender);
        remove_transaction(index);
    }
}


// Something wrong about this
// If timeout everything should fail and no need for messages
void Transaction::handle_timeout_error(int index) {

    if(transactionConnections[index].state == Transaction::STATE_SENDING_NODE) {
        numberOfReceivedCapacityErrors++;
        // Remove the transaction from the list
        // Or try a different path
    } else if(transactionConnections[index].state == Transaction::STATE_FORWARDING_NODE) {

        update_message_buf(Transaction::timeout_error(transactionConnections[index].transId),
                                                                 transactionConnections[index].edgeTowardsSender);

        update_message_buf(Transaction::timeout_error(transactionConnections[index].transId),
                                                                 transactionConnections[index].edgeTowardsReceiver);
        remove_transaction(index);
    } else if(transactionConnections[index].state == Transaction::STATE_RECEIVING_NODE ) {


        update_message_buf(Transaction::timeout_error(transactionConnections[index].transId),
                                                                 transactionConnections[index].edgeTowardsSender);
        remove_transaction(index);

    }

}

void Transaction::handle_general_error(int index) {

}

// TODO add msgDelay to the bufferedmessage process
//  or add it later to the game somehow the way it works now
//  is a little shitty
void Transaction::update_message_buf(BasicMessage *msg, int outgoingEdge) {
    msgDelay = latency.calculate_delay_ms();
    // msgDelay = msgDelay + 0.5;
    BufferedMessage * bufMsg = new BufferedMessage(msg, outgoingEdge, msgDelay);
    msgBuf.addMessage(bufMsg);
}

int Transaction::get_trans_conn_index(int transId) {
    if(transactionConnectionIndex > 0) {
        for(int i = 0; i < transactionConnectionIndex; i++) {
            if(transactionConnections[i].transId == transId) {
                return i;
            }
        }
    }
    return -1;
}

void Transaction::remove_transaction(int index) {
    if(index < 0) {
        return;
    }

    if(index == 1) {
        transactionConnectionIndex = 0;
    } else {
        for(int i = index; i < transactionConnectionIndex; i++) {
            if((i+1) < transactionConnectionIndex) {
                transactionConnections[i] = transactionConnections[i+1];
            }
        }
        transactionConnectionIndex--;
    }

}

// TODO rename
// Initial transaction query/trial/something
// this message gets forwarded
BasicMessage * Transaction::initial_send_request(int endNode, int amount, int transactionId, int neighbourhood) {
    char msgname[40];
    sprintf(msgname, "Initial transaction message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(Transaction::MESSAGE_TYPE);
    msg->setSubType(Transaction::TRANSACTION_QUERY);

    msg->setEndNodeId(endNode);
    msg->setAmount(amount);
    msg->setTransactionId(transactionId);

    msg->setNeighbourhoodIndex(neighbourhood);

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

BasicMessage * Transaction::transfer_payment(int transactionId) {
    char msgname[40];
    sprintf(msgname, "Transfer Payment");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(Transaction::MESSAGE_TYPE);
    msg->setSubType(Transaction::TRANSACTION_PUSH);

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

BasicMessage * Transaction::close_transaction(int transactionId) {
    char msgname[40];
    sprintf(msgname, "Finalize Transaction");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(Transaction::MESSAGE_TYPE);
    msg->setSubType(Transaction::CLOSE_TRANSACTION);

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

BasicMessage * Transaction::capacity_error(int transactionId) {
    char msgname[40];
    sprintf(msgname, "Capacity Error");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(Transaction::MESSAGE_TYPE);
    msg->setSubType(Transaction::CAPACITY_ERROR);

    msg->setTransactionId(transactionId);

    return msg;
}

BasicMessage * Transaction::timeout_error(int transactionId) {
    char msgname[40];
    sprintf(msgname, "Timeout error");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(Transaction::MESSAGE_TYPE);
    msg->setSubType(Transaction::TIMEOUT);

    msg->setTransactionId(transactionId);

    return msg;
}

BasicMessage * Transaction::general_error(int transactionId) {
    char msgname[40];
    sprintf(msgname, "Error error");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(Transaction::MESSAGE_TYPE);
    msg->setSubType(Transaction::ERROR);

    msg->setTransactionId(transactionId);

    return msg;
}

// This failure should never happen
// Need to write protocol if it does.
BasicMessage * Transaction::transaction_failed(int transactionId, int amount, int endNode, int neighbourhood) {
    char msgname[40];
    sprintf(msgname, "Error error");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(Transaction::MESSAGE_TYPE);
    msg->setSubType(Transaction::ERROR);

    msg->setTransactionId(transactionId);

    msg->setNeighbourhoodIndex(neighbourhood);

    return msg;
}

std::string Transaction::to_string(void) {

    LinkedNode *node = connectedNeighbours[currentNeighbour].get_upstream_linked_node(21, 10);

    return node->to_string(); // connectedNeighbours[0].to_string();
}

std::string Transaction::capacities_to_string(void) {

    // bool checkCapacity = connectedNeighbours[currentNeighbour].check_capacity(endNode, amount);

    return connectedNeighbours[0].get_all_capacities();
}

std::string Transaction::delay_to_string(void) {
    return std::to_string(msgDelay);
}


