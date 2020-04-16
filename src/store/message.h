#pragma once
// LANGUAGE: CwC
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;
// author: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu

// Enum representing the message kind
enum class MsgKind
{
    Ack = 0, Nack = 1, Put = 2, Reply = 3, Get = 4,
    WaitAndGet = 5, Status = 6, Kill = 7, Register = 8, Directory = 9
};

/** Class representing a message that has fields representing the message kind,
 * sender index, target index, and id
 */ 
class Message : public Object
{
public:
    MsgKind kind_; // the message kind
    size_t sender_; // the index of the sender node
    size_t target_; // the index of the receiver node
    size_t id_; // an id t unique within the node
    
    // Constructor
    Message(MsgKind kind, size_t sender, size_t target, size_t id) {
        kind_ = kind;
        sender_ = sender;
        target_ = target;
        id_ = id;
    }

    // Constructor used during deserializing where fields represents the serialized data
    Message(MsgKind kind, char* fields) {
        String f(fields);
        char* str_chr = f.c_str();

        char *sender = strtok_r(str_chr, ";", &str_chr); // get sender from data
        sender_ = (size_t)atoi(sender); // convert char sender to size_t

        char *target = strtok_r(str_chr, ";", &str_chr); // split on semicolon
        target_ = (size_t)atoi(target); // convert char target to size_t

        char *id = strtok_r(str_chr, ";", &str_chr); // split on semicolon
        id_ = (size_t)atoi(id); // convert char target to size_t

        kind_ = kind;
    }

    virtual ~Message() {

    }

    // Method to serialize a message, with this format:
    // 2;sender_index;target_index;id;type_number;
    char* serialize() { 
        string msg_serialize("2;"); // indicates that it is a message
        msg_serialize += to_string(static_cast<int>(kind_)) + semicolon; // add type to serialization
        msg_serialize += to_string(sender_) + semicolon;// add sender index to serialization
        msg_serialize += to_string(target_) + semicolon;// add receiver index to serialization
        msg_serialize += to_string(id_) + semicolon;// add id to serialization

        char* char_array = new char[msg_serialize.length() + 1]; 
        strcpy(char_array, msg_serialize.c_str());
        return char_array;
    }
};

// Class that represents an Ack message
class Ack : public Message
{
public:
    // Default Constructor
    Ack(size_t sender, size_t target, size_t id) : Message(MsgKind::Ack, sender, target, id) {
    }

    // Constructor used for deserializing, receives a String representing the serialized data
    Ack(char* fields) : Message(MsgKind::Ack, fields) { }

    // Destructor
    ~Ack() { }

    /** Equals method */
    bool equals(Object  * other) {
        if (other == nullptr) return false;
		Ack* s = dynamic_cast<Ack*>(other);
		if (s == nullptr) return false;

		return (sender_ == s->sender_) && (target_ == s->target_) && (id_ == s->id_);
    }
};

// Class to represent a status message
class Status : public Message
{
public:
    String *msg_; // owned

    // Constructor
    Status(size_t sender, size_t target, size_t id, String *msg) : Message(MsgKind::Status, sender, target, id) {
        msg_ = msg;
    }

    // Constructor used for deserializing, receives a String representing the serialized data
    Status(char* fields) : Message(MsgKind::Status, fields) {
        // Skip the first 3 fields
        size_t num_fields = 0;
        char *token = std::strtok(fields, ";"); 
        while (num_fields < 3 && token != NULL) {
            token = std::strtok(NULL, ";");
            num_fields++;
        }
        
        msg_ = new String(token);
    }

    // Destructor
    ~Status() {
        delete msg_;
    }

    // Method to serialize a status message, with this format:
    // 2;sender_index;target_index;id;type_number;msg;
    char* serialize() {
        char* p1 = Message::serialize();
        string msg_serialize = string(p1) + string(msg_->c_str()) + semicolon;

        char* char_array = new char[msg_serialize.length() + 1]; 
        strcpy(char_array, msg_serialize.c_str());
        delete [] p1;
        return char_array;
    }

    /** Equals method */
    bool equals(Object  * other) {
        if (other == nullptr) return false;
		Status* s = dynamic_cast<Status*>(other);
		if (s == nullptr) return false;

		return (sender_ == s->sender_) && (target_ == s->target_) && (id_ == s->id_) && msg_->equals(s->msg_);
    }
};

// Class to represent a register message
class Register : public Message
{
public:
    struct sockaddr_in slave;
    size_t port;
    char const * ip; // represent the ip address to be used in slave
    size_t slave_port; // represent the port to be used in hte slave

    // Constructor
    Register(size_t sender, size_t target, size_t id, size_t pt, char const * slave_ip, size_t slave_port_given) : Message(MsgKind::Register, sender, target, id) {
        port = pt;
        ip = slave_ip;
        slave_port = slave_port_given;
        slave.sin_family = AF_INET;
        slave.sin_addr.s_addr = inet_addr(ip);
        slave.sin_port = htons( slave_port );
    }

    // Constructor used for deserializing, receives a String representing the serialized data
    Register(char* fields) : Message(MsgKind::Status, fields) {
        // Skip the first 3 fields
        size_t num_fields = 0;
        char *token = std::strtok(fields, ";"); 
        while (num_fields < 3 && token != NULL) {
            token = std::strtok(NULL, ";");
            num_fields++;
        }
        port = (size_t)atoi(token); // convert char port to size_t

        ip = std::strtok(NULL, ";"); // next field is ip
        char* slave_port_char = std::strtok(NULL, ";");
        slave_port = (size_t)atoi(slave_port_char);

        // initialize the sock sin attributes
        slave.sin_family = AF_INET;
        slave.sin_addr.s_addr = inet_addr(ip);
        slave.sin_port = htons( slave_port );
    }

    // Destructor
    ~Register() { }

    // Method to serialize register message, with this format:
    // 2;sender_index;target_index;id;type_number;port;slave_ip;slave_port;
    char* serialize() {
        char* p1 = Message::serialize();
        string msg_serialize = string(p1);
        // add port to serialized message
        msg_serialize += to_string(port) + semicolon;
        // adds the slave ip for sockaddr_in
        msg_serialize += string(ip) + semicolon;
        // adds the slave port for sockaddr_in
        msg_serialize += to_string(slave_port) + semicolon;

        char* char_array = new char[msg_serialize.length() + 1]; 
        strcpy(char_array, msg_serialize.c_str());
        delete [] p1;
        return char_array;
    }

    /** Equals method */
    bool equals(Object  * other) {
        if (other == nullptr) return false;
		Register* s = dynamic_cast<Register*>(other);
		if (s == nullptr) return false;

		bool msg_equal = ((sender_ == s->sender_) && (target_ == s->target_) && (id_ == s->id_));
        bool slave_equal = ((slave_port == s->slave_port) && (port == s->port)); //&& strip.equals(&other_strip));
        return msg_equal && slave_equal;
    }
    
};

class Directory : public Message
{
public:
    size_t slaves;
    size_t num_ports;
    size_t *ports; // owned
    StringArray *addresses; // owned; strings owned

    // Constructor
    Directory(size_t sender, size_t target, size_t id, size_t sl, size_t *pt, size_t num_ports_given, StringArray *addr) : Message(MsgKind::Directory, sender, target, id) {
        slaves = sl;
        num_ports = num_ports_given;
        ports = new size_t[num_ports];
        for (size_t i = 0; i < num_ports; i++) {
            ports[i] = pt[i];
        }
        addresses = new StringArray(*addr);
    }

    // Constructor used for deserializing, receives a String representing the serialized data
    Directory(char* fields) : Message(MsgKind::Status, fields) {
        // Skip the first 3 fields
        size_t num_fields = 0;
        char* str_chr = fields;
        char *token = strtok_r(str_chr, ";", &str_chr);
        
        // even though there are 6 semicolons before string, it's 8 include the stringarray type after 
        while (num_fields < 3 && token != NULL) {
            token = strtok_r(str_chr, ";", &str_chr);
            num_fields++;
        }
        // Create char pointer array to get all fields
        StringArray* rest_fields = new StringArray(2);
        for (size_t i = 0; i < 3; i++) {
            String* f = new String(token);
            rest_fields->add(f);
            token = strtok_r(str_chr, ";", &str_chr);
        }

        slaves = (size_t)atoi(rest_fields->get(0)->c_str()); // convert char to size_t
        num_ports = (size_t)atoi(rest_fields->get(1)->c_str()); // convert char to size_t
        ports = new size_t[num_ports];

        // will only add ports up to num_ports, anything more will be discarded
        char* ports_chr = rest_fields->get(2)->c_str();
        token = strtok_r(ports_chr, ",", &ports_chr);
        for (size_t i = 0; i < num_ports; i++) {
            ports[i] = (size_t)atoi(token);
            token = strtok_r(ports_chr, ",", &ports_chr);
        }
        String addrstring(str_chr);
        addresses = new StringArray(&addrstring);
        delete rest_fields;
    }

    // Destructor
    ~Directory() {
        delete addresses;
        delete [] ports;
    }

    // Method to serialize directory message, with this format:
    // 2;sender_index;target_index;id;slaves;num_ports;port1,port2,...,;slave_ports=addresses;
    char* serialize() {
        char* p1 = Message::serialize();
        string msg_serialize = string(p1);
        // adds the slaves
        msg_serialize += to_string(slaves) + semicolon;
        // adds the number of ports
        msg_serialize += to_string(num_ports) + semicolon;

        // add ports serialized
        for (size_t i = 0; i < num_ports; i++) { 
            msg_serialize += to_string(ports[i]) + comma;
        }
        char* addr_serialized = addresses->serialize();
        msg_serialize += semicolon + string(addr_serialized) + semicolon;

        char* char_array = new char[msg_serialize.length() + 1]; 
        strcpy(char_array, msg_serialize.c_str());
        delete [] p1; delete [] addr_serialized;
        return char_array;
    }

    /** Equals method */
    bool equals(Object  * other) {
        if (other == nullptr) return false;
		Directory* s = dynamic_cast<Directory*>(other);
		if (s == nullptr) return false;

		bool msg_equal = ((sender_ == s->sender_) && (target_ == s->target_) && (id_ == s->id_));
        bool slave_equal = ((slaves == s->slaves) && (num_ports == s->num_ports));
        // Check if the first 5 fields are equal
        if (!(msg_equal && slave_equal)) {
            return false;
        }

        // Check if the ports are the same
        for (size_t i = 0; i < num_ports; i++) {
            if (ports[i] != s->ports[i]) {
                return false;
            }
        }

        // Equality now depends on the string array of addresses
        return addresses->equals(s->addresses);
    }
};