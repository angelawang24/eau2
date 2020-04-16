#pragma once
// LANGUAGE: CwC

#include "../store/message.h"
#include "../util/array.h"
#include "../client/dataframe.h"

// author: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
// Represents a serial class that allows for objects to be serialized and deserialized
class Serial {
public:
    // Serialize a string array
    static char* serialize(Object* str) {
        return str->serialize();
    }

    // Returns object that serialized char represents
    static Object* deserialize(char* str) {
        String serialized(str);
        if (serialized.size() == 0) { return nullptr; }
        String* type_char = serialized.substring(0,0);
        String* elements = serialized.substring(2, serialized.size() - 1);

        String* s0 = new String("0");
        String* s1 = new String("1");
        String* s2 = new String("2");
        String* s3 = new String("3");
        Object* r = nullptr;
        
        if (type_char->equals(s0)) { // string array to deserialize
            r = new StringArray(elements);
        } else if (type_char->equals(s1)) { // double array to deserialize
            r = new DoubleArray(elements);
        } else if (type_char->equals(s2)) { // message to deserialize
            r = create_message_(elements);
        } else if (type_char->equals(s3)) { // dataframe to deserialize
            r = create_dataframe_(elements);
        }
        delete s0; delete s1; delete s2; delete s3;
        delete type_char; delete elements;
        return r;
    }

    // Creates a message the rest of the serialized string
    static Message* create_message_(String* str) {
        char* str_chr = str->c_str();
        char *type = strtok_r(str_chr, ";", &str_chr); // get message type from data
        int msg_type = atoi(type); // convert message type to int

        Message* r = nullptr;
        if (msg_type == 0) {
            r = new Ack(str_chr);
        } else if (msg_type == 6) {
            r = new Status(str_chr);
        } else if (msg_type == 8) {
            r = new Register(str_chr);
        } else if (msg_type == 9) {
            r = new Directory(str_chr);
        }
        return r;
    }

    static DataFrame* create_dataframe_(String* str) {
        // ISBF;2;0,1,;hey,hey,;1,1,;1.20000,1.20000,;
        char *schema = std::strtok(str->c_str(), ";"); // get schema from data
        String* fields = str->substring(strlen(schema)+1, str->size() - 1);

        Schema s(schema);
        DataFrame* df = new DataFrame(s, fields);
        delete fields;
        return df;
    }
};