#include "src/alg/spanning_tree/SpanningTree.h"

SpanningTree::SpanningTree(){
    sent_requests = 0;
    edgesWeightUpdated = false;
    spanningTreeNodeId= rand();
    stateNode = SpanningTree::STATE_SLEEPING;

    // TODO check if needs to be initialized with best-weight right out of box
    fragmentName = spanningTreeNodeId;

    currentMinEdge = -1;
    testEdge = -1;
    bestEdge = -1;
    bestWeight = std::numeric_limits<int>::max();
    findCount = 0;
    edgeTowardsRoot = -1;

    initialQueueIndex = -1;
    connectQueueIndex = 0;
    reportQueueIndex = 0;
    testQueueIndex = 0;

    testCounter = 0;
    handle_message_counter = 0;
    handle_spanning_tree_counter = 0;
    handle_queue = 0;
    maxQueue = 0;

    isRoot = false;

    msgSentDownStream = 0;
    msgDelay = 0.0;

    reply_broadcast = "";

    previousSimTime = 0;
}

void SpanningTree::set_neighbours(Neighbours *n)
{
    connectedNeighbours = n;
    numConnectedNodes = connectedNeighbours->get_amount_of_neighbours();
    int *list_ptr = &listOfOutGatesRand[0];
    connectedNeighbours->fill_array_with_random_gate_list(list_ptr);
    fill_state_edges(&stateEdges[0]);
}

int SpanningTree::get_num_neighbours(void){
    return numConnectedNodes;
}

int SpanningTree::get_num_sent_messages(void){
    return sent_requests;
}

int SpanningTree::get_initial_queue_index(void) {
    return initialQueueIndex;
}

int SpanningTree::get_level(void) {
    return fragmentLevel;
}

void SpanningTree::fill_state_edges(state_edge *state_edges) {
    for(int i = 0; i < numConnectedNodes; i++) {
        // (state_edges + i)->outgoing_edge = listOfOutGatesRand[i];
        (state_edges + i)->outgoing_edge = i;
        (state_edges + i)->state = SpanningTree::EDGESTATE_Q_MST;
        (state_edges + i)->weight = std::numeric_limits<int>::max();
    }
}

void SpanningTree::set_node_id(int id){
    nodeId = id;
}

bool SpanningTree::is_node_root(void)
{
    return isRoot;
}

int SpanningTree::get_state(void) {
    return stateNode;
}

std::string SpanningTree::get_state_edge(void) {

    std::string res = "";

    if(stateNode == SpanningTree::STATE_FIND) {
        res = res + "Node in State: Find \n";
    } else if(stateNode == SpanningTree::STATE_FOUND) {
        res = res + "Node in State: Found \n";
    } else {
        res = res + "Node in State: Other? \n";
    }

    char room[256];
    for(int i = 0; i < numConnectedNodes; i++) {
        sprintf( room, "\n %d outoging_edge: %d \n weight: %d \n", i, stateEdges[i].outgoing_edge, stateEdges[i].weight);
        res = res + room;
        if(stateEdges[i].state == SpanningTree::EDGESTATE_Q_MST) {
            res = res + " state: ? MST";
        } else if(stateEdges[i].state == SpanningTree::EDGESTATE_IN_MST) {
            res = res + " state: In MST";
        } else if(stateEdges[i].state == SpanningTree::EDGESTATE_NOT_IN_MST){
            res = res + " state: not in mst";
        } else {
            char buf[40];
            sprintf(buf, " state: %d", stateEdges[i].state);
            res = res + buf;
        }
        res = res + " \n";

    }
    char bug[40];
    sprintf(bug, "Towards root: %d \n", edgeTowardsRoot);

    char queue[300];
    sprintf(queue, " queue connect: %d \n queue test: %d \n queue report: %d \n testCounter: %d \n fragmentName: %d \n initialQueueIndex: %d \n handleMessage: %d \n handleSpanningTree: %d \n handleQueue: %d \n maxQueue: %d \n", connectQueueIndex, testQueueIndex, reportQueueIndex, testCounter, fragmentName, initialQueueIndex,handle_message_counter, handle_spanning_tree_counter, handle_queue, maxQueue);
    res = res +  bug + " \n" + queue;

    if(isRoot) {
        res = res + " I am the root \n";
    }
    if(edgesWeightUpdated) {
        res = res + " edgesWeights updated!! \n";
    }

    return res;

}

void SpanningTree::wake_up(void) {
    get_weight_edges();
    stateNode = SpanningTree::STATE_FINDING_WEIGHT;
}

void SpanningTree::start_building_tree(void) {

    currentMinEdge = find_minimum_weight_edge();
    bestWeight = stateEdges[currentMinEdge].weight;
    fragmentName = bestWeight;
    stateEdges[currentMinEdge].state = SpanningTree::EDGESTATE_IN_MST;
    fragmentLevel = 0;
    stateNode = SpanningTree::STATE_FOUND;
    findCount = 0;

    // fix??
    edgeTowardsRoot = stateEdges[currentMinEdge].outgoing_edge;

    update_message_buf(SpanningTree::connect(fragmentLevel), stateEdges[currentMinEdge].outgoing_edge);
}

void SpanningTree::get_weight_edges(void) {
    for(int i = 0; i < numConnectedNodes; i++) {
        update_message_buf(SpanningTree::weight_request(spanningTreeNodeId, i), stateEdges[i].outgoing_edge);
        sent_requests++;
    }
}

int SpanningTree::get_message_count(void)
{
    return msgBuf.getMessageCount();
}

BufferedMessage * SpanningTree::get_message(void)
{
    return msgBuf.getMessage();
}


void SpanningTree::handle_message(BasicMessage* msg, int outgoingEdge, omnetpp::simtime_t prevSimTime)
{
    handle_message_counter++;
    if(previousSimTime != prevSimTime) {
        msgDelay = 0;
    } else {
        previousSimTime = prevSimTime;
    }


    if(stateNode == SpanningTree::STATE_SLEEPING) {
        wake_up();
    }

    if(edgesWeightUpdated) {
//        if(initialQueueIndex > -1){
//            msgDelay = 0;
//            handle_initial_queue();
//            return;
//        }
        handle_spanning_tree_message(msg, outgoingEdge);


    } else {
        if(msg->getSubType() == SpanningTree::WEIGHT_REQUEST) {
            handle_weight_request(outgoingEdge, msg->getSpanningTreeId(), msg->getSpanningTreeIndexList());
        } else if(msg->getSubType() == SpanningTree::WEIGHT_REPONSE) {
            handle_weight_response(msg->getWeightEdgeSpanningTree(), msg->getSpanningTreeIndexList());
        } else {
            QueuedMessage* initialQueueElement = new QueuedMessage();
            BasicMessage *msg_p = new BasicMessage();
            msg_p = msg->dup();
            initialQueueElement->set_initial_queue(msg_p, outgoingEdge);
            initialQueueIndex++;
            maxQueue++;
            initialQueue[initialQueueIndex] = initialQueueElement;
        }
    }

    check_queued_messages();
}

void SpanningTree::handle_initial_queue(void) {
    if(edgesWeightUpdated){
        while(initialQueueIndex > -1) {
            initialQueueIndex--;
            if(initialQueueIndex > -1){
                handle_queue++;
                BasicMessage *msg = initialQueue[initialQueueIndex]->get_message();
                int outgoingEdge = initialQueue[initialQueueIndex]->get_edge();
                handle_spanning_tree_message(msg, outgoingEdge);
                //delete(initialQueue[initialQueueIndex]);
            }
        }
    }


}

void SpanningTree::handle_spanning_tree_message(BasicMessage *msg, int outgoingEdge) {
    handle_spanning_tree_counter++;
    if(msg->getSubType() == SpanningTree::CONNECT) {
       handle_connect(outgoingEdge, msg->getLevelNode());
    } else if(msg->getSubType() == SpanningTree::INITIATE) {
       handle_initiate(outgoingEdge, msg->getLevelNode(), msg->getFragmentName(), msg->getNodeState());
    } else if(msg->getSubType() == SpanningTree::TEST) {
       handle_test(outgoingEdge, msg->getLevelNode(), msg->getFragmentName());
    } else if(msg->getSubType() == SpanningTree::ACCEPT) {
       handle_accept(outgoingEdge);
    } else if(msg->getSubType() == SpanningTree::REJECT) {
       handle_reject(outgoingEdge);
    } else if(msg->getSubType() == SpanningTree::REPORT) {
       handle_report(outgoingEdge, msg->getBestWeight());
    } else if(msg->getSubType() == SpanningTree::CHANGEROOT) {
       change_root();
    } else if(msg->getSubType() == SpanningTree::DOWNTREE_BROADCAST) {
       handle_downtree_broadcast();
    } else if(msg->getSubType() == SpanningTree::UPTREE_REPLY) {
       handle_uptree_reply();
    } else if(msg->getSubType() == SpanningTree::ROOT_QUERY) {
       handle_root_query(outgoingEdge, msg->getSpanningTreeId());
    } else if(msg->getSubType() == SpanningTree::ROOT_QUERY_ACCEPT) {
       isRoot = true;
    } else if(msg->getSubType() == SpanningTree::ROOT_QUERY_REJECT) {
        isRoot = false;
    } else if(msg->getSubType() == SpanningTree::WEIGHT_REQUEST) {
        handle_weight_request(outgoingEdge, msg->getSpanningTreeId(), msg->getSpanningTreeIndexList());
    } else if(msg->getSubType() == SpanningTree::WEIGHT_REPONSE) {
        handle_weight_response(msg->getWeightEdgeSpanningTree(), msg->getSpanningTreeIndexList());
    }
}

void SpanningTree::handle_weight_request(int outgoingEdge, int spanningTreeId, int index) {
    int weight;
    if(spanningTreeNodeId >= spanningTreeId) {
        weight = spanningTreeNodeId + 5 - spanningTreeId;
    } else {
        weight = spanningTreeId - spanningTreeNodeId;
    }
    update_message_buf(SpanningTree::weight_response(weight, index), outgoingEdge);
}

void SpanningTree::handle_weight_response(int weight, int index) {
    stateEdges[index].weight = weight;
    sent_requests--;

    if(sent_requests == 0) {
        edgesWeightUpdated = true;
        start_building_tree();
        if(initialQueueIndex > -1){
            msgDelay = 0;
            handle_initial_queue();
            return;
        }
    }
}

void SpanningTree::handle_connect(int outgoingEdge, int level) {
    // If still in weight response stage add message to queue

    // int pos = find_edge_in_stateEdge(outgoingEdge);
    int pos = outgoingEdge;

    if(level < fragmentLevel) {
        stateEdges[pos].state = SpanningTree::EDGESTATE_IN_MST;
        update_message_buf(SpanningTree::initiate(fragmentLevel, fragmentName, stateNode), outgoingEdge);
        if (stateNode == SpanningTree::STATE_FIND) {
            findCount++;
        }
    } else {
        if(stateEdges[pos].state == SpanningTree::EDGESTATE_Q_MST) {
            QueuedMessage* connectElement = new QueuedMessage();
            connectElement->set_connect_message(SpanningTree::CONNECT, outgoingEdge, level);
            connectQueue[connectQueueIndex] = connectElement;
            connectQueueIndex++;
        } else {
            fragmentName = stateEdges[pos].weight;
            fragmentLevel = fragmentLevel + 1;

            // stateEdges[pos].state = SpanningTree::EDGESTATE_IN_MST;
            //int level_send = fragmentLevel + 1;

            update_message_buf(SpanningTree::initiate(fragmentLevel, fragmentName, SpanningTree::STATE_FIND), outgoingEdge);
        }
    }
}

void SpanningTree::handle_initiate(int outgoingEdge, int level, int fragName, int nodState) {
    fragmentLevel = level;
    fragmentName = fragName;
    stateNode = nodState;
    edgeTowardsRoot = outgoingEdge;

    bestEdge = -1;
    bestWeight = std::numeric_limits<int>::max();

    for(int i = 0; i < numConnectedNodes; i++) {
        if(stateEdges[i].outgoing_edge != outgoingEdge) {
            if(stateEdges[i].state == SpanningTree::EDGESTATE_IN_MST) {
                update_message_buf(SpanningTree::initiate(fragmentLevel, fragmentName, stateNode), stateEdges[i].outgoing_edge);
                if(nodState == SpanningTree::STATE_FIND) {
                    findCount++;
                }
            }
        }
    }
    if(nodState == SpanningTree::STATE_FIND) {
        test();
    }

}

void SpanningTree::handle_test(int outgoingEdge, int level, int fragName) {

    if(level > fragmentLevel) {
        QueuedMessage* testElement = new QueuedMessage();
        testElement->set_test_message(SpanningTree::TEST, outgoingEdge, level, fragName);
        testQueue[testQueueIndex] = testElement;
        testQueueIndex++;
    } else {
        if(fragName != fragmentName) {
            update_message_buf(SpanningTree::accept(), outgoingEdge);
        } else {
            // int pos = find_edge_in_stateEdge(outgoingEdge);
            int pos = outgoingEdge;
            if(stateEdges[pos].state == SpanningTree::EDGESTATE_Q_MST) {
                stateEdges[pos].state = SpanningTree::EDGESTATE_NOT_IN_MST;
            }

            if(testEdge != pos) {
                update_message_buf(SpanningTree::reject(), outgoingEdge);
            } else {
                test();
            }
        }
    }
}

void SpanningTree::handle_accept(int outgoingEdge) {
    testEdge = -1;
    //int pos = find_edge_in_stateEdge(outgoingEdge);
    int pos = outgoingEdge;
    if(stateEdges[pos].weight < bestWeight) {
        bestEdge = pos;
        bestWeight = stateEdges[pos].weight;
    }
    report();
}

void SpanningTree::handle_reject(int outgoingEdge) {
    //int pos = find_edge_in_stateEdge(outgoingEdge);
    int pos = outgoingEdge;
    if(stateEdges[pos].state == SpanningTree::EDGESTATE_Q_MST and edgeTowardsRoot != outgoingEdge) {
        stateEdges[pos].state = SpanningTree::EDGESTATE_NOT_IN_MST;
        testEdge = -1;

    }
    test();
}

void SpanningTree::handle_report(int outgoingEdge, int bestW) {
    if(outgoingEdge != edgeTowardsRoot) {
        findCount--;
        if(bestW < bestWeight) {
            bestWeight = bestW;
            bestEdge = outgoingEdge;
        }
        report();
    } else {
        if(stateNode == SpanningTree::STATE_FIND) {
            QueuedMessage* reportElement = new QueuedMessage();
            reportElement->set_report_message(SpanningTree::REPORT, outgoingEdge, bestW);
            reportQueue[reportQueueIndex] = reportElement;
            reportQueueIndex++;


        } else {
            if(bestW > bestWeight) {
                change_root();
            } else {
                if(bestW == std::numeric_limits<int>::max()) {
                    check_root();
                }
            }
        }
    }
}

void SpanningTree::handle_downtree_broadcast(void) {
    if(not broadcast_down_stream()) {
        broadcast_up_stream();
    }
}

void SpanningTree::handle_uptree_reply(void) {
    msgSentDownStream--;
    bool broadcast_up = true;
    if(msgSentDownStream == 0) {
        broadcast_up = broadcast_up_stream();
    }

    if(not broadcast_up) {
        broadcast();
    }
}

void SpanningTree::handle_root_query(int outgoingEdge, int node_id) {
    if(outgoingEdge == stateEdges[edgeTowardsRoot].outgoing_edge and spanningTreeNodeId > node_id) {
        update_message_buf(SpanningTree::root_query_accept(), outgoingEdge);
    } else {
        update_message_buf(SpanningTree::root_query_reject(), outgoingEdge);
    }
}

void SpanningTree::test(void) {
    testCounter++;
    int bestW = std::numeric_limits<int>::max();
    bool noEdgeInQMST = true;
    for(int i = 0; i < numConnectedNodes; i++) {
        if(stateEdges[i].state == SpanningTree::EDGESTATE_Q_MST and
                stateEdges[i].outgoing_edge != edgeTowardsRoot) {
            noEdgeInQMST = false;
            if(bestW > stateEdges[i].weight){
                testEdge = stateEdges[i].outgoing_edge;
                bestW = stateEdges[i].weight;
            }
        }
    }

    if (noEdgeInQMST) {
        testEdge = -1;
    }

    if(testEdge > -1) {
        update_message_buf(SpanningTree::test(fragmentLevel, fragmentName), testEdge);
    } else {
        testEdge = -1;
        report();
    }
}

void SpanningTree::report(void) {
    if(findCount == 0 and testEdge == -1) {
        stateNode = SpanningTree::STATE_FOUND;
        update_message_buf(SpanningTree::report(bestWeight), edgeTowardsRoot);
    }
}

void SpanningTree::change_root(void) {
    // fix?
    if(stateEdges[bestEdge].state == SpanningTree::EDGESTATE_NOT_IN_MST) {
        bestEdge = -1;
    }

    if(bestEdge > -1) {
        // int pos = find_edge_in_stateEdge(bestEdge);
        int pos = bestEdge;
        if(stateEdges[pos].state == SpanningTree::EDGESTATE_IN_MST){
            update_message_buf(SpanningTree::change_root_msg(), bestEdge);
        } else {
            update_message_buf(SpanningTree::connect(fragmentLevel), bestEdge);
            stateEdges[pos].state = SpanningTree::EDGESTATE_IN_MST;
        }
    }

}

int SpanningTree::find_minimum_weight_edge(void) {
    int min_weight = stateEdges[0].weight;
    int pos = 0;
    for(int i = 1; i < numConnectedNodes; i++) {
        if(min_weight > stateEdges[i].weight) {
            min_weight = stateEdges[i].weight;
            pos = i;
        }
    }
    return pos;
}

int SpanningTree::find_edge_in_stateEdge(int incomingEdge) {
    int pos = -1;
    for(int i = 0; i < numConnectedNodes; i++) {
        if(stateEdges[i].outgoing_edge == incomingEdge) {
            return i;
        }
    }
    return pos;
}

void SpanningTree::send_inspection(int num, int inspect) {
    for(int i = 0; i < num; i++) {
        update_message_buf(SpanningTree::inspection(inspect), i);
    }
}

void SpanningTree::check_root(void) {
    update_message_buf(SpanningTree::root_query(spanningTreeNodeId), edgeTowardsRoot);
}

void SpanningTree::broadcast(void) {
    if(msgSentDownStream == 0 and isRoot == true) {
        broadcast_down_stream();
    }
}

bool SpanningTree::broadcast_down_stream(void) {
    bool msgSent = false;

    if(isRoot) {
        for(int i = 0; i < numConnectedNodes; i++) {
                    if(stateEdges[i].state == SpanningTree::EDGESTATE_IN_MST) {
                        update_message_buf(SpanningTree::broadcast_down_tree(), stateEdges[i].outgoing_edge);
                        msgSent = true;
                        msgSentDownStream++;
                    }
                }
    } else {
        for(int i = 0; i < numConnectedNodes; i++) {
                    if(stateEdges[i].state == SpanningTree::EDGESTATE_IN_MST and
                            stateEdges[i].outgoing_edge != edgeTowardsRoot) {
                        update_message_buf(SpanningTree::broadcast_down_tree(), stateEdges[i].outgoing_edge);
                        msgSent = true;
                        msgSentDownStream++;
                    }
                }
    }


    return msgSent;
}


void SpanningTree::check_queued_messages(void) {

    if(initialQueueIndex > -1 and edgesWeightUpdated){
        handle_initial_queue();
    }

    if(connectQueueIndex >= 0) {
        for(int i = 0; i < connectQueueIndex; i++) {
            int edge = connectQueue[i]->get_edge();
            int lvl = connectQueue[i]->get_level();

            if(check_connect_queue_conditions(edge, lvl)) {
                handle_connect(edge, lvl);

                //Remove from list and shift list
                for(int j = i; j < connectQueueIndex; j++) {
                    connectQueue[j] = connectQueue[j + 1];
                }
                connectQueueIndex--;
                if(connectQueueIndex > 0){
                    if(i > 0) {
                        i--;
                    }
                }
            }
        }
    }

    if(reportQueueIndex >= 0) {
        for(int i = 0; i < reportQueueIndex; i++) {
            int edge = reportQueue[i]->get_edge();
            int weight = reportQueue[i]->get_weight();

            if(check_report_queue_conditions(edge, weight)) {
                handle_report(edge, weight);

                //Remove from list and shift list
                for(int j = i; j < reportQueueIndex; j++) {
                    reportQueue[j] = reportQueue[j + 1];
                }
                reportQueueIndex--;
                if(reportQueueIndex > 0){
                    if(i > 0) {
                        i--;
                    }
                }
            }
        }
    }

    if(testQueueIndex >= 0) {
        for(int i = 0; i < testQueueIndex; i++) {
            int lvl = testQueue[i]->get_level();

            if(check_test_queue_conditions(lvl)) {
                handle_test(testQueue[i]->get_edge(), lvl, testQueue[i]->get_fragment_name());

                //Remove from list and shift list
                for(int j = i; j < testQueueIndex; j++) {
                    testQueue[j] = testQueue[j + 1];
                }
                testQueueIndex--;
                if(testQueueIndex > 0){
                    if(i > 0) {
                        i--;
                    }
                }
            }
        }
    }
}

bool SpanningTree::broadcast_up_stream(void) {
    bool msgSent = false;

    if(not isRoot){
        for(int i = 0; i < numConnectedNodes; i++) {
                    if(stateEdges[i].state == SpanningTree::EDGESTATE_IN_MST and
                            (stateEdges[i].outgoing_edge == edgeTowardsRoot)) {
                        update_message_buf(SpanningTree::broadcast_up_tree(), stateEdges[i].outgoing_edge);
                        msgSent = true;
                    }
                }
    }

    return msgSent;
}

// Returns true if message can be removed from queue
bool SpanningTree::check_connect_queue_conditions(int edge, int lvl) {
    if(lvl < fragmentLevel) {
        return true;
    } else {
        //int pos = find_edge_in_stateEdge(edge);
        int pos = edge;
        if(stateEdges[pos].state == SpanningTree::EDGESTATE_Q_MST){
            return false;
        } else {
            return true;
        }
    }
}

// Returns true if message can be removed from queue
bool SpanningTree::check_report_queue_conditions(int edge, int weight) {
    if (edge != edgeTowardsRoot){
        return true;
    } else {
        if(stateNode == SpanningTree::STATE_FIND) {
            return false;
        } else {
            return true;
        }
    }
}

// Return true if message can be removed from queue
bool SpanningTree::check_test_queue_conditions(int level) {
    if(level > fragmentLevel) {
        return false;
    } else {
        return true;
    }
}

void SpanningTree::update_message_buf(BasicMessage *msg, int outgoingEdge) {
    msgDelay = msgDelay + 0.05;
    BufferedMessage * buf_msg = new BufferedMessage(msg, outgoingEdge, msgDelay);
    msgBuf.addMessage(buf_msg);
}

BasicMessage * SpanningTree::inspection(int inspect) {
    char msgname[80];
    sprintf(msgname, "Inspection %d", inspect);
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::INSPECTION);
    msg->setInspection(inspect);

    return msg;
}

BasicMessage * SpanningTree::weight_request(int spanningTreeId, int indexList) {
    char msgname[40];
    sprintf(msgname, "weight request");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::WEIGHT_REQUEST);
    msg->setSpanningTreeId(spanningTreeId);
    msg->setSpanningTreeIndexList(indexList);

    return msg;
}

BasicMessage * SpanningTree::weight_response(int weight, int indexListRequest) {
    char msgname[80];
    sprintf(msgname, "weight response: %d", weight);
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::WEIGHT_REPONSE);
    msg->setWeightEdgeSpanningTree(weight);
    msg->setSpanningTreeIndexList(indexListRequest);

    return msg;
}

BasicMessage * SpanningTree::connect(int level) {
    char msgname[40];
    sprintf(msgname, "Connect message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::CONNECT);
    msg->setLevelNode(level);

    return msg;
}

BasicMessage * SpanningTree::initiate(int level, int fragmentName, int stateNode) {
    char msgname[80];
    sprintf(msgname, "Initiate message, lvl: %d, fragN: %d", level, fragmentName);
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::INITIATE);
    msg->setLevelNode(level); // LN
    msg->setFragmentName(fragmentName); //FN
    msg->setNodeState(stateNode); //SN

    return msg;
}

BasicMessage * SpanningTree::test(int level, int fragmentName) {
    char msgname[40];
    sprintf(msgname, "Test message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::TEST);
    msg->setLevelNode(level); // LN
    msg->setFragmentName(fragmentName); //FN

    return msg;
}

BasicMessage * SpanningTree::accept(void) {
    char msgname[40];
    sprintf(msgname, "Accept message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::ACCEPT);

    return msg;
}

BasicMessage * SpanningTree::reject(void) {
    char msgname[40];
    sprintf(msgname, "Reject message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::REJECT);

    return msg;
}

BasicMessage * SpanningTree::report(int bestW) {
    char msgname[40];
    sprintf(msgname, "Report message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::REPORT);
    msg->setBestWeight(bestW);

    return msg;
}

BasicMessage * SpanningTree::change_root_msg() {
    char msgname[40];
    sprintf(msgname, "Change Root message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::CHANGEROOT);

    return msg;
}

BasicMessage * SpanningTree::broadcast_down_tree(void) {
    char msgname[40];
    sprintf(msgname, "BroadCast message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::DOWNTREE_BROADCAST);

    return msg;
}

BasicMessage * SpanningTree::broadcast_up_tree(void) {
    char msgname[40];
    sprintf(msgname, "BroadCast Reply message");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::UPTREE_REPLY);

    return msg;
}

BasicMessage * SpanningTree::root_query(int spanningTreeId) {
    char msgname[40];
    sprintf(msgname, "root query");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::ROOT_QUERY);
    msg->setSpanningTreeId(spanningTreeId);

    return msg;
}

BasicMessage * SpanningTree::root_query_accept(void) {
    char msgname[40];
    sprintf(msgname, "root accept");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::ROOT_QUERY_ACCEPT);

    return msg;
}

BasicMessage * SpanningTree::root_query_reject(void) {
    char msgname[40];
    sprintf(msgname, "root reject");
    BasicMessage *msg = new BasicMessage(msgname);

    msg->setType(SpanningTree::MESSAGE_TYPE);
    msg->setSubType(SpanningTree::ROOT_QUERY_REJECT);

    return msg;
}
