//
// Generated file, do not edit! Created by nedtool 5.4 from basicmsg.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __BASICMSG_M_H
#define __BASICMSG_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>basicmsg.msg:17</tt> by nedtool.
 * <pre>
 * message BasicMessage
 * {
 *     int source;
 *     int destination;
 *     int scalar_clock;
 *     int src_node_id;
 * 
 *     int pulseNum;
 * 
 *     int type;
 *     int subType;
 * 
 *     // Spanning Tree Params:
 *     int spanningTreeId;
 *     int spanningTreeIndexList;
 *     int weightEdgeSpanningTree;
 * 
 *     int levelNode;
 *     int fragmentName;
 *     int nodeState;
 *     int bestWeight;
 * 
 *     string replyBroadcast;
 *     // Otherthings...
 * 
 *     int inspection;
 * 
 *     int lowestIdFragment;
 *     int rootNodeId;
 * 
 *     int leaderId;
 * 
 *     bool ack;
 * 
 *     int root_node;
 *     bool start_spanning_tree = false;
 * 
 *     bool spanning_request = false;
 * 
 *     bool spanning_request_ack = false;
 *     int spanning_tree_level;
 * 
 *     bool spanning_decline_request = false;
 * 
 *     bool down_broadcast = false;
 *     bool up_broadcast_reply = false;
 * 
 * }
 * </pre>
 */
class BasicMessage : public ::omnetpp::cMessage
{
  protected:
    int source;
    int destination;
    int scalar_clock;
    int src_node_id;
    int pulseNum;
    int type;
    int subType;
    int spanningTreeId;
    int spanningTreeIndexList;
    int weightEdgeSpanningTree;
    int levelNode;
    int fragmentName;
    int nodeState;
    int bestWeight;
    ::omnetpp::opp_string replyBroadcast;
    int inspection;
    int lowestIdFragment;
    int rootNodeId;
    int leaderId;
    bool ack;
    int root_node;
    bool start_spanning_tree;
    bool spanning_request;
    bool spanning_request_ack;
    int spanning_tree_level;
    bool spanning_decline_request;
    bool down_broadcast;
    bool up_broadcast_reply;

  private:
    void copy(const BasicMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const BasicMessage&);

  public:
    BasicMessage(const char *name=nullptr, short kind=0);
    BasicMessage(const BasicMessage& other);
    virtual ~BasicMessage();
    BasicMessage& operator=(const BasicMessage& other);
    virtual BasicMessage *dup() const override {return new BasicMessage(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getSource() const;
    virtual void setSource(int source);
    virtual int getDestination() const;
    virtual void setDestination(int destination);
    virtual int getScalar_clock() const;
    virtual void setScalar_clock(int scalar_clock);
    virtual int getSrc_node_id() const;
    virtual void setSrc_node_id(int src_node_id);
    virtual int getPulseNum() const;
    virtual void setPulseNum(int pulseNum);
    virtual int getType() const;
    virtual void setType(int type);
    virtual int getSubType() const;
    virtual void setSubType(int subType);
    virtual int getSpanningTreeId() const;
    virtual void setSpanningTreeId(int spanningTreeId);
    virtual int getSpanningTreeIndexList() const;
    virtual void setSpanningTreeIndexList(int spanningTreeIndexList);
    virtual int getWeightEdgeSpanningTree() const;
    virtual void setWeightEdgeSpanningTree(int weightEdgeSpanningTree);
    virtual int getLevelNode() const;
    virtual void setLevelNode(int levelNode);
    virtual int getFragmentName() const;
    virtual void setFragmentName(int fragmentName);
    virtual int getNodeState() const;
    virtual void setNodeState(int nodeState);
    virtual int getBestWeight() const;
    virtual void setBestWeight(int bestWeight);
    virtual const char * getReplyBroadcast() const;
    virtual void setReplyBroadcast(const char * replyBroadcast);
    virtual int getInspection() const;
    virtual void setInspection(int inspection);
    virtual int getLowestIdFragment() const;
    virtual void setLowestIdFragment(int lowestIdFragment);
    virtual int getRootNodeId() const;
    virtual void setRootNodeId(int rootNodeId);
    virtual int getLeaderId() const;
    virtual void setLeaderId(int leaderId);
    virtual bool getAck() const;
    virtual void setAck(bool ack);
    virtual int getRoot_node() const;
    virtual void setRoot_node(int root_node);
    virtual bool getStart_spanning_tree() const;
    virtual void setStart_spanning_tree(bool start_spanning_tree);
    virtual bool getSpanning_request() const;
    virtual void setSpanning_request(bool spanning_request);
    virtual bool getSpanning_request_ack() const;
    virtual void setSpanning_request_ack(bool spanning_request_ack);
    virtual int getSpanning_tree_level() const;
    virtual void setSpanning_tree_level(int spanning_tree_level);
    virtual bool getSpanning_decline_request() const;
    virtual void setSpanning_decline_request(bool spanning_decline_request);
    virtual bool getDown_broadcast() const;
    virtual void setDown_broadcast(bool down_broadcast);
    virtual bool getUp_broadcast_reply() const;
    virtual void setUp_broadcast_reply(bool up_broadcast_reply);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const BasicMessage& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, BasicMessage& obj) {obj.parsimUnpack(b);}


#endif // ifndef __BASICMSG_M_H

