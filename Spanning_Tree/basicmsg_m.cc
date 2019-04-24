//
// Generated file, do not edit! Created by nedtool 5.4 from basicmsg.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "basicmsg_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp


// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(BasicMessage)

BasicMessage::BasicMessage(const char *name, short kind) : ::omnetpp::cMessage(name,kind)
{
    this->source = 0;
    this->destination = 0;
    this->scalar_clock = 0;
    this->src_node_id = 0;
    this->type = 0;
    this->leaderId = 0;
    this->ack = false;
    this->root_node = 0;
    this->start_spanning_tree = false;
    this->spanning_request = false;
    this->spanning_request_ack = false;
    this->spanning_tree_level = 0;
    this->spanning_decline_request = false;
    this->down_broadcast = false;
    this->up_broadcast_reply = false;
}

BasicMessage::BasicMessage(const BasicMessage& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

BasicMessage::~BasicMessage()
{
}

BasicMessage& BasicMessage::operator=(const BasicMessage& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void BasicMessage::copy(const BasicMessage& other)
{
    this->source = other.source;
    this->destination = other.destination;
    this->scalar_clock = other.scalar_clock;
    this->src_node_id = other.src_node_id;
    this->type = other.type;
    this->leaderId = other.leaderId;
    this->ack = other.ack;
    this->root_node = other.root_node;
    this->start_spanning_tree = other.start_spanning_tree;
    this->spanning_request = other.spanning_request;
    this->spanning_request_ack = other.spanning_request_ack;
    this->spanning_tree_level = other.spanning_tree_level;
    this->spanning_decline_request = other.spanning_decline_request;
    this->down_broadcast = other.down_broadcast;
    this->up_broadcast_reply = other.up_broadcast_reply;
}

void BasicMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->source);
    doParsimPacking(b,this->destination);
    doParsimPacking(b,this->scalar_clock);
    doParsimPacking(b,this->src_node_id);
    doParsimPacking(b,this->type);
    doParsimPacking(b,this->leaderId);
    doParsimPacking(b,this->ack);
    doParsimPacking(b,this->root_node);
    doParsimPacking(b,this->start_spanning_tree);
    doParsimPacking(b,this->spanning_request);
    doParsimPacking(b,this->spanning_request_ack);
    doParsimPacking(b,this->spanning_tree_level);
    doParsimPacking(b,this->spanning_decline_request);
    doParsimPacking(b,this->down_broadcast);
    doParsimPacking(b,this->up_broadcast_reply);
}

void BasicMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->source);
    doParsimUnpacking(b,this->destination);
    doParsimUnpacking(b,this->scalar_clock);
    doParsimUnpacking(b,this->src_node_id);
    doParsimUnpacking(b,this->type);
    doParsimUnpacking(b,this->leaderId);
    doParsimUnpacking(b,this->ack);
    doParsimUnpacking(b,this->root_node);
    doParsimUnpacking(b,this->start_spanning_tree);
    doParsimUnpacking(b,this->spanning_request);
    doParsimUnpacking(b,this->spanning_request_ack);
    doParsimUnpacking(b,this->spanning_tree_level);
    doParsimUnpacking(b,this->spanning_decline_request);
    doParsimUnpacking(b,this->down_broadcast);
    doParsimUnpacking(b,this->up_broadcast_reply);
}

int BasicMessage::getSource() const
{
    return this->source;
}

void BasicMessage::setSource(int source)
{
    this->source = source;
}

int BasicMessage::getDestination() const
{
    return this->destination;
}

void BasicMessage::setDestination(int destination)
{
    this->destination = destination;
}

int BasicMessage::getScalar_clock() const
{
    return this->scalar_clock;
}

void BasicMessage::setScalar_clock(int scalar_clock)
{
    this->scalar_clock = scalar_clock;
}

int BasicMessage::getSrc_node_id() const
{
    return this->src_node_id;
}

void BasicMessage::setSrc_node_id(int src_node_id)
{
    this->src_node_id = src_node_id;
}

int BasicMessage::getType() const
{
    return this->type;
}

void BasicMessage::setType(int type)
{
    this->type = type;
}

int BasicMessage::getLeaderId() const
{
    return this->leaderId;
}

void BasicMessage::setLeaderId(int leaderId)
{
    this->leaderId = leaderId;
}

bool BasicMessage::getAck() const
{
    return this->ack;
}

void BasicMessage::setAck(bool ack)
{
    this->ack = ack;
}

int BasicMessage::getRoot_node() const
{
    return this->root_node;
}

void BasicMessage::setRoot_node(int root_node)
{
    this->root_node = root_node;
}

bool BasicMessage::getStart_spanning_tree() const
{
    return this->start_spanning_tree;
}

void BasicMessage::setStart_spanning_tree(bool start_spanning_tree)
{
    this->start_spanning_tree = start_spanning_tree;
}

bool BasicMessage::getSpanning_request() const
{
    return this->spanning_request;
}

void BasicMessage::setSpanning_request(bool spanning_request)
{
    this->spanning_request = spanning_request;
}

bool BasicMessage::getSpanning_request_ack() const
{
    return this->spanning_request_ack;
}

void BasicMessage::setSpanning_request_ack(bool spanning_request_ack)
{
    this->spanning_request_ack = spanning_request_ack;
}

int BasicMessage::getSpanning_tree_level() const
{
    return this->spanning_tree_level;
}

void BasicMessage::setSpanning_tree_level(int spanning_tree_level)
{
    this->spanning_tree_level = spanning_tree_level;
}

bool BasicMessage::getSpanning_decline_request() const
{
    return this->spanning_decline_request;
}

void BasicMessage::setSpanning_decline_request(bool spanning_decline_request)
{
    this->spanning_decline_request = spanning_decline_request;
}

bool BasicMessage::getDown_broadcast() const
{
    return this->down_broadcast;
}

void BasicMessage::setDown_broadcast(bool down_broadcast)
{
    this->down_broadcast = down_broadcast;
}

bool BasicMessage::getUp_broadcast_reply() const
{
    return this->up_broadcast_reply;
}

void BasicMessage::setUp_broadcast_reply(bool up_broadcast_reply)
{
    this->up_broadcast_reply = up_broadcast_reply;
}

class BasicMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    BasicMessageDescriptor();
    virtual ~BasicMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(BasicMessageDescriptor)

BasicMessageDescriptor::BasicMessageDescriptor() : omnetpp::cClassDescriptor("BasicMessage", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

BasicMessageDescriptor::~BasicMessageDescriptor()
{
    delete[] propertynames;
}

bool BasicMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<BasicMessage *>(obj)!=nullptr;
}

const char **BasicMessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *BasicMessageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int BasicMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 15+basedesc->getFieldCount() : 15;
}

unsigned int BasicMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<15) ? fieldTypeFlags[field] : 0;
}

const char *BasicMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "source",
        "destination",
        "scalar_clock",
        "src_node_id",
        "type",
        "leaderId",
        "ack",
        "root_node",
        "start_spanning_tree",
        "spanning_request",
        "spanning_request_ack",
        "spanning_tree_level",
        "spanning_decline_request",
        "down_broadcast",
        "up_broadcast_reply",
    };
    return (field>=0 && field<15) ? fieldNames[field] : nullptr;
}

int BasicMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "source")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destination")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "scalar_clock")==0) return base+2;
    if (fieldName[0]=='s' && strcmp(fieldName, "src_node_id")==0) return base+3;
    if (fieldName[0]=='t' && strcmp(fieldName, "type")==0) return base+4;
    if (fieldName[0]=='l' && strcmp(fieldName, "leaderId")==0) return base+5;
    if (fieldName[0]=='a' && strcmp(fieldName, "ack")==0) return base+6;
    if (fieldName[0]=='r' && strcmp(fieldName, "root_node")==0) return base+7;
    if (fieldName[0]=='s' && strcmp(fieldName, "start_spanning_tree")==0) return base+8;
    if (fieldName[0]=='s' && strcmp(fieldName, "spanning_request")==0) return base+9;
    if (fieldName[0]=='s' && strcmp(fieldName, "spanning_request_ack")==0) return base+10;
    if (fieldName[0]=='s' && strcmp(fieldName, "spanning_tree_level")==0) return base+11;
    if (fieldName[0]=='s' && strcmp(fieldName, "spanning_decline_request")==0) return base+12;
    if (fieldName[0]=='d' && strcmp(fieldName, "down_broadcast")==0) return base+13;
    if (fieldName[0]=='u' && strcmp(fieldName, "up_broadcast_reply")==0) return base+14;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *BasicMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "bool",
        "int",
        "bool",
        "bool",
        "bool",
        "int",
        "bool",
        "bool",
        "bool",
    };
    return (field>=0 && field<15) ? fieldTypeStrings[field] : nullptr;
}

const char **BasicMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *BasicMessageDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int BasicMessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    BasicMessage *pp = (BasicMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *BasicMessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    BasicMessage *pp = (BasicMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string BasicMessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    BasicMessage *pp = (BasicMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSource());
        case 1: return long2string(pp->getDestination());
        case 2: return long2string(pp->getScalar_clock());
        case 3: return long2string(pp->getSrc_node_id());
        case 4: return long2string(pp->getType());
        case 5: return long2string(pp->getLeaderId());
        case 6: return bool2string(pp->getAck());
        case 7: return long2string(pp->getRoot_node());
        case 8: return bool2string(pp->getStart_spanning_tree());
        case 9: return bool2string(pp->getSpanning_request());
        case 10: return bool2string(pp->getSpanning_request_ack());
        case 11: return long2string(pp->getSpanning_tree_level());
        case 12: return bool2string(pp->getSpanning_decline_request());
        case 13: return bool2string(pp->getDown_broadcast());
        case 14: return bool2string(pp->getUp_broadcast_reply());
        default: return "";
    }
}

bool BasicMessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    BasicMessage *pp = (BasicMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setSource(string2long(value)); return true;
        case 1: pp->setDestination(string2long(value)); return true;
        case 2: pp->setScalar_clock(string2long(value)); return true;
        case 3: pp->setSrc_node_id(string2long(value)); return true;
        case 4: pp->setType(string2long(value)); return true;
        case 5: pp->setLeaderId(string2long(value)); return true;
        case 6: pp->setAck(string2bool(value)); return true;
        case 7: pp->setRoot_node(string2long(value)); return true;
        case 8: pp->setStart_spanning_tree(string2bool(value)); return true;
        case 9: pp->setSpanning_request(string2bool(value)); return true;
        case 10: pp->setSpanning_request_ack(string2bool(value)); return true;
        case 11: pp->setSpanning_tree_level(string2long(value)); return true;
        case 12: pp->setSpanning_decline_request(string2bool(value)); return true;
        case 13: pp->setDown_broadcast(string2bool(value)); return true;
        case 14: pp->setUp_broadcast_reply(string2bool(value)); return true;
        default: return false;
    }
}

const char *BasicMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *BasicMessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    BasicMessage *pp = (BasicMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}


