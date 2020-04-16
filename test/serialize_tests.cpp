#include "../src/util/string.h"
#include "../src/util/array.h"
#include "../src/util/serial.h"
#include <iostream>

// author: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
void FAIL() { exit(1); }
void OK(const char *m)
{
    Sys *c = new Sys();
	c->p("OK: ");
	c->pln(m);
    delete c;
}
void t_true(bool p)
{
    if (!p)
        FAIL();
}
void t_false(bool p)
{
    if (p)
        FAIL();
}

void testSubstring() {
    String s("Hello");
    String sub("Hell");
    String sub2("ell");
    String sub3("ello");

    String* s1actual = s.substring(0, 3);
    String* s2actual= s.substring(1, 3);
    String* s3actual = s.substring(1, s.size() - 1);
    t_true(s1actual->equals(&sub));
    t_true(s2actual->equals(&sub2));
    t_true(s3actual->equals(&sub3));

    String s2("0;0;");
    String sub2a("0;");
    String* s4actual = s2.substring(2, s2.size() - 1);
    t_true(s4actual->equals(&sub2a));

    delete s1actual; delete s2actual; delete s3actual; delete s4actual;
    OK("Substring");
}

void testStringArraySerialize()
{
    StringArray* s1 = new StringArray();
    StringArray* s2 = new StringArray();
    String *s = new String("Hello");
    String *t = new String("World");
    String *u = new String("HelloWorld");

    s2->add(s);
    s2->add(t);
    s2->add(u);

    char* empty_serialize = const_cast<char*>("0;0;");
    char* non_empty = const_cast<char*>("0;3;Hello,World,HelloWorld,");
    char* empty_actual = s1->serialize();
    char* nonempty_actual = s2->serialize();

    t_true(strncmp(empty_actual, empty_serialize, strlen(empty_serialize)) == 0);
    OK("Empty string array serialize");
    t_true(strncmp(nonempty_actual, non_empty, strlen(non_empty)) == 0);
    delete s1;
    delete s2;
    delete [] empty_actual;
    delete [] nonempty_actual;
    OK("Non-empty string array serialize");
}

void testDoubleArraySerialize()
{
    DoubleArray* s1 = new DoubleArray();
    DoubleArray* s2 = new DoubleArray();

    s2->add(1.001);
    s2->add(12.103002412312412);
    s2->add(3.4);

    char* empty_serialize = const_cast<char*>("1;0;");
    char* non_empty = const_cast<char*>("1;3;1.001000,12.103002,3.400000,");
    char* empty_actual = s1->serialize();
    char* nonempty_actual = s2->serialize();

    t_true(strncmp(empty_actual, empty_serialize, strlen(empty_serialize)) == 0);
    OK("Empty double array serialize");

    t_true(strncmp(nonempty_actual, non_empty, strlen(non_empty)) == 0);
    delete s1;
    delete s2;
    delete [] empty_actual;
    delete [] nonempty_actual;
    OK("Non-empty double array serialize");
}

void testAckSerialize()
{
    Ack a(1, 2, 3);
    
    char* serialize = const_cast<char*>("2;0;1;2;3;");
    // 2-message to serialize, 0-message type, 1-sender, 2-target, 3-id
    char* actual = a.serialize();
    t_true(strncmp(actual, serialize, strlen(serialize)) == 0);
    delete [] actual;
    OK("Ack serialize");
}

void testStatusSerialize()
{
    String* msg = new String("Why am I not learning anything");
    Status a(1, 2, 3, msg);
    
    char* serialize = const_cast<char*>("2;6;1;2;3;Why am I not learning anything;");
    // 2-message to serialize, 6-message type, 1-sender, 2-target, 3-id, message at end
    char* actual = a.serialize();
    t_true(strncmp(actual, serialize, strlen(serialize)) == 0);
    delete [] actual;
    OK("Status serialize");
}

void testRegisterSerialize()
{
    char const * ip = "127.0.0.1";
    Register a(1, 2, 3, 8080, ip, 8081);
    
    char* serialize = const_cast<char*>("2;8;1;2;3;8080;127.0.0.1;8081;");
    // 2-message to serialize, 8-message type, 1-sender, 2-target, 3-id, 8080-port, 127.0.0.1-slaveip, 8080-slaveport
    char* actual = a.serialize();
    t_true(strncmp(actual, serialize, strlen(serialize)) == 0);
    delete [] actual;
    OK("Register serialize");
}

void testDirectorySerialize()
{
    size_t slaves = 3;
    size_t *ports = new size_t[3];
    ports[0] = 8081;
    ports[1] = 8082;
    ports[2] = 8083;
    StringArray *addresses = new StringArray(); // owned; strings owned
    String *s1 = new String("127.0.0.1");
    String *s2 = new String("127.0.0.2");
    String *s3 = new String("127.0.0.3");
    addresses->add(s1);
    addresses->add(s2);
    addresses->add(s3);
    Directory a(1, 2, 3, slaves, ports, 3, addresses);
    
    char* serialize = const_cast<char*>("2;9;1;2;3;3;3;8081,8082,8083,;0;3;127.0.0.1,127.0.0.2,127.0.0.3,;");
    // 2-message to serialize, 8-message type, 1-sender, 2-target, 3-id
    // 3-number of ports, 8081...8083-ports
    // 0-string array to serialize, 3-size of array, 127.0.0.1...-ips to serialize
    char* actual = a.serialize();
    t_true(strncmp(actual, serialize, strlen(serialize)) == 0);
    delete [] actual; delete [] ports; delete addresses;
    OK("Directory serialize");
}

void testDataFrameSerialize()
{
    Schema s("ISBF");

    DataFrame df(s);

    Row r(df.get_schema());
    String str("hey");
    for(size_t i = 0; i <  2; i++) {
        r.set(0,(int)i);
        r.set(1, &str);
        r.set(2,(bool)true);
        r.set(3,(double)1.2);
        df.add_row(r);
    }

    char* serialize = const_cast<char*>("3;ISBF;2;0,1,;hey,hey,;1,1,;1.200000,1.200000,;");
    char* actual = df.serialize();
    t_true(strncmp(actual, serialize, strlen(serialize)) == 0);
    delete [] actual;
    OK("DataFrame serialize");
}

void testSerial() {
    size_t slaves = 3;
    size_t *ports = new size_t[3];
    ports[0] = 8081;
    ports[1] = 8082;
    ports[2] = 8083;
    StringArray* addresses = new StringArray(); // owned; strings owned
    String *s1 = new String("127.0.0.1");
    String *s2 = new String("127.0.0.2");
    String *s3 = new String("127.0.0.3");
    addresses->add(s1);
    addresses->add(s2);
    addresses->add(s3);
    Directory a(1, 2, 3, slaves, ports, 3, addresses);
    
    char* serialize = const_cast<char*>("2;9;1;2;3;3;3;8081,8082,8083,;0;3;127.0.0.1,127.0.0.2,127.0.0.3,;");
    // 2-message to serialize, 8-message type, 1-sender, 2-target, 3-id
    // 3-number of ports, 8081...8083-ports
    // 0-string array to serialize, 3-number of elements, 127.0.0.1...-ips to serialize
    char* actual = Serial::serialize(&a);
    t_true(strncmp(actual, serialize, strlen(serialize)) == 0);
    delete [] actual;
    OK("Serial serialize on directory");

    StringArray* sa1 = new StringArray();
    StringArray* sa2 = new StringArray();
    String *s = new String("Hello");
    String *t = new String("World");
    String *u = new String("HelloWorld");

    sa2->add(s);
    sa2->add(t);
    sa2->add(u);

    char* empty_serialize = const_cast<char*>("0;0;");
    char* non_empty = const_cast<char*>("0;3;Hello,World,HelloWorld,");

    char* empty_actual = Serial::serialize(sa1);
    char* nonempty_actual = Serial::serialize(sa2);
    t_true(strncmp(empty_actual, empty_serialize, strlen(empty_serialize)) == 0);
    OK("Serial serialize on empty string array");
    t_true(strncmp(nonempty_actual, non_empty, strlen(non_empty)) == 0);
    delete [] empty_actual;
    delete [] nonempty_actual;
    delete addresses; delete sa1; delete sa2; delete [] ports;
    OK("Serial serialize on non-empty string array");
}

void testStringArrayDeserialize()
{
    StringArray* s1 = new StringArray();
    StringArray* s2 = new StringArray();
    String *s = new String("Hello");
    String *t = new String("World");
    String *u = new String("HelloWorld");

    s2->add(s);
    s2->add(t);
    s2->add(u);

    char* empty_serialize = const_cast<char*>("0;0;");
    char* non_empty = const_cast<char*>("0;3;Hello,World,HelloWorld,");

    Object* empty_actual = Serial::deserialize(empty_serialize);
    Object* nonempty_actual = Serial::deserialize(non_empty);

    t_true(empty_actual->equals(s1));
    OK("Empty string array deserialize");
    t_true(nonempty_actual->equals(s2));

    delete empty_actual; delete nonempty_actual;
    delete s1; delete s2;
    OK("Non-empty string array deserialize");
}

void testDoubleArrayDeserialize()
{
    DoubleArray* s2 = new DoubleArray();

    s2->add(1.001);
    s2->add(12.1030);
    s2->add(3.4);

    char* empty_serialize = const_cast<char*>("1;0;");
    char* non_empty = const_cast<char*>("1;3;1.001000,12.103000,3.400000,");

    Object* empty = Serial::deserialize(empty_serialize);
    char* empty_actual = empty->serialize();
    Object* non = Serial::deserialize(non_empty);
    char* nonempty_actual = non->serialize();

    t_true(strncmp(empty_actual, empty_serialize, strlen(empty_serialize)) == 0);
    OK("Empty double array deserialize");
    t_true(strncmp(nonempty_actual, non_empty, strlen(non_empty)) == 0);

    delete s2; delete [] empty_actual; delete [] nonempty_actual; delete empty; delete non;
    OK("Non-empty double array deserialize");
}

void testAckDeserialize()
{
    Ack a(1, 2, 3);
    
    char* serialize = const_cast<char*>("2;0;1;2;3;");
    Object* actual = Serial::deserialize(serialize);
    // 2-message to serialize, 0-message type, 1-sender, 2-target, 3-id
    t_true(actual->equals(&a));
    delete actual;
    OK("Ack deserialize");
}

void testStatusDeserialize()
{
    String* msg = new String("Why am I not learning anything");
    Status a(1, 2, 3, msg);
    
    char* serialize = const_cast<char*>("2;6;1;2;3;Why am I not learning anything;");
    Object* actual = Serial::deserialize(serialize);
    // 2-message to serialize, 6-message type, 1-sender, 2-target, 3-id, message at end
    t_true(actual->equals(&a));
    delete actual;

    OK("Status deserialize");
}

void testRegisterDeserialize()
{
    char const * ip = "127.0.0.1";
    Register a(1, 2, 3, 8080, ip, 8081);
    
    char* serialize = const_cast<char*>("2;8;1;2;3;8080;127.0.0.1;8081;");
    // 2-message to serialize, 8-message type, 1-sender, 2-target, 3-id, 8080-port, 127.0.0.1-slaveip, 8080-slaveport
    Object* actual = Serial::deserialize(serialize);
    t_true(actual->equals(&a));
    delete actual;
    OK("Register deserialize");
}

void testDirectoryDeserialize()
{
    size_t slaves = 3;
    size_t *ports = new size_t[3];
    ports[0] = 8081;
    ports[1] = 8082;
    ports[2] = 8083;
    StringArray *addresses = new StringArray(); // owned; strings owned
    String *s1 = new String("127.0.0.1");
    String *s2 = new String("127.0.0.2");
    String *s3 = new String("127.0.0.3");
    addresses->add(s1);
    addresses->add(s2);
    addresses->add(s3);
    Directory a(1, 2, 3, slaves, ports, 3, addresses);
    
    char* serialize = const_cast<char*>("2;9;1;2;3;3;3;8081,8082,8083,;0;3;127.0.0.1,127.0.0.2,127.0.0.3,;");
    // 2-message to serialize, 8-message type, 1-sender, 2-target, 3-id
    // 3-number of ports, 8081...8083-ports
    // 0-string array to serialize, 3-size of array, 127.0.0.1...-ips to serialize

    Object* actual = Serial::deserialize(serialize);
    t_true(a.equals(&a));
    delete actual; delete [] ports; delete addresses;
    OK("Directory deserialize");
}

void testDataFrameDeserialize()
{
    Schema s("ISBF");

    DataFrame df(s);

    Row r(df.get_schema());
    String str("hey");
    for(size_t i = 0; i <  2; i++) {
        r.set(0,(int)i);
        r.set(1, &str);
        r.set(2,(bool)true);
        r.set(3,(double)1.2);
        df.add_row(r);
    }

    char* serialize = const_cast<char*>("3;ISBF;2;0,1,;hey,hey,;1,1,;1.200000,1.200000,;");
    Object* actualobj = Serial::deserialize(serialize);
    char* actual = actualobj->serialize();
    t_true(strncmp(actual, serialize, strlen(serialize)) == 0);
    delete [] actual; delete actualobj;
    OK("DataFrame deserialize");
}

void testSerializeDeserialize() {
    size_t slaves = 3;
    size_t *ports = new size_t[3];
    ports[0] = 8081;
    ports[1] = 8082;
    ports[2] = 8083;
    StringArray *addresses = new StringArray(); // owned; strings owned
    String *s1 = new String("127.0.0.1");
    String *s2 = new String("127.0.0.2");
    String *s3 = new String("127.0.0.3");
    addresses->add(s1);
    addresses->add(s2);
    addresses->add(s3);
    Directory a(1, 2, 3, slaves, ports, 3, addresses);
    // 2-message to serialize, 8-message type, 1-sender, 2-target, 3-id
    // 3-number of ports, 8081...8083-ports
    // 0-string array to serialize, 3-size of array, 127.0.0.1...-ips to serialize

    char* serialized = Serial::serialize(&a);
    Object* actual = Serial::deserialize(serialized);
    t_true(actual->equals(&a));
    delete [] serialized; delete actual; delete [] ports; delete addresses;
    OK("Directory serialize deserialize");
}

int main() {
    testSubstring();
    testStringArraySerialize();
    testDoubleArraySerialize();
    testAckSerialize();
    testStatusSerialize();
    testRegisterSerialize();
    testDirectorySerialize();
    testDataFrameSerialize();
    testSerial();

    testStringArrayDeserialize();
    testDoubleArrayDeserialize();
    testAckDeserialize();
    testStatusDeserialize();
    testRegisterDeserialize();
    testDirectoryDeserialize();
    testDataFrameDeserialize();
    testSerializeDeserialize();
    return 0;
}