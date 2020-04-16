#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <arpa/inet.h>
#include <poll.h>

#define MAXCLIENT 256
extern int errno;

// author: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
/** 
 * Represents any networking device. Contains information on an ip address and how many clients this 
 * networking device knows about on the same network. 
 */

class NetworkingDevice {
public:
    char* ip;
    int num_clients;

    NetworkingDevice() {
        ip = "0.0.0.0";
        num_clients = 0;
    }

    ~NetworkingDevice() {
        delete [] ip;
    }

    //performs a poll on the given file descriptor to tell whether there's something to read from the socket
    bool do_poll_(int fd, int timeout) {
        struct pollfd poll_struct;
        poll_struct.fd = fd;
        poll_struct.events = POLLIN;
        int retval;

        retval  = poll(&poll_struct, 1, timeout);
        if(retval < 0) {
            std::cout << "there has been an error with poll" << std::endl;
            exit(1);
        }

        return poll_struct.revents && POLLIN; 
    }

    //connects to some device and returns the socket used to connect with it
    int connect_to_someone_(const char* ip_to_connect, int port_to_connect) {
        int errnum;

        int sock = 0;
        struct sockaddr_in addr_;
        assert((sock = socket(AF_INET, SOCK_STREAM, 0)) >= 0);
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port_to_connect);

        // Convert IP addresses from text to binary form, use ip address to send to 127.0.0.1
        assert(inet_pton(AF_INET, ip_to_connect, &addr_.sin_addr.s_addr) > 0);
        int conn = connect(sock, (struct sockaddr *)&addr_, sizeof(addr_));
        errnum = errno;
        if (errno == 111) {
            std::cout << "start the server before starting the client" << std::endl;
            exit(1);
        }
        
        assert(conn >= 0);
        return sock;
        // should make a map of IP addresses to open connections
    }


    //creates a socket to listen on that other clients can connect on 
    int create_my_socket_(struct sockaddr_in* adr, socklen_t* addrlen, int port) {
        int sock;
        int opt = 1;

        // Creating socket file descriptor
        assert((sock = socket(AF_INET, SOCK_STREAM, 0)) != 0);

        //setting properties of the socket
        int setsock = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        adr->sin_family = AF_INET;
        adr->sin_addr.s_addr = inet_addr(ip);
        adr->sin_port = htons(port);
       
        // Attaching socket to the port 8080
        assert((bind(sock, (sockaddr*)adr, *addrlen)) >= 0);
        assert((listen(sock, num_clients)) >= 0);
        std::cout << "created a socket to listen on" << std::endl;

        return sock;

    }



};

// author: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
/** 
 * Represents a server class. Contains information on its own ip address, the number of
 * clients, the client ip addresses and the client socket
 */
class Server  : public NetworkingDevice {
public:
    char const * ip;
    int port;
    size_t num_clients;
    StringArray* client_ips;
    StringArray* client_ports;
    StringArray* client_messages;
    IntArray* clients_done;
    int* client_sock;
    int num_clients_done;

    Server (char* ip_arg, int port_arg) {
        ip = ip_arg;
        port = port_arg;
        num_clients = 0;
        client_ips = new StringArray();
        client_ports = new StringArray();
        client_messages = new StringArray();
        clients_done = new IntArray();
        //supports up to 256
        client_sock = new int[MAXCLIENT];
    }

    ~Server() {
        delete [] ip;
        delete client_ips;
        delete client_ports;
        delete [] client_sock;


    }

    // To initialize the server and set up the socket to communicate with other clients
    void run_server () {
        int errnum;

        int sock;
        struct sockaddr_in adr;
        int addrlen = sizeof(adr);
        int server_fd = create_my_socket_((struct sockaddr_in*)&adr, (socklen_t*)&addrlen, port);

        char buffer[1024] = {0};
        std::cout << "server is waiting for connections " << std::endl;


        //listen for clients attempting to connect and store their IP addresses
        bool teardown = false;
        while (!teardown) {
            if(do_poll_(server_fd, 200)){
                accept_and_parse_message_(server_fd, (struct sockaddr*)&adr, (socklen_t*)&addrlen);

            }
            listen_for_done_messages();
            //if we have gotten a done message from each client, we teardown
            if(num_clients != 0 && clients_done->size() == num_clients) {teardown = true;}
        }

        std::cout << "server has gotten all done messages and is initiating teardown" << std::endl;

        for(int i = 0; i < num_clients; i++) {
            send(client_sock[i], "teardown", 8, 0);  
        }

        std::cout << "sent all teardown messages" << std::endl;


        //now listen for a client OK from all the clients -- will mean they all got the teardown notification, 
        //then we can safely actually do a teardown

        int num_ok = 0;
        char buffer2[1024] = {0};


        while(num_ok < num_clients) {
            for (int i = 0; i < num_clients; i++) {
                int valread = read(client_sock[i], buffer2, 1024); 
                if((strncmp(buffer2, "OK", 2)) == 0){
                    std::cout << "got a ok message from client #" << i << std::endl;;
                    num_ok++;
                } 
            }
        }

        //actually do a teardown -- close all sockets
        close(server_fd);
        for(int i = 0; i < num_clients; i++) {
            close(client_sock[i]);
        }

        std::cout << "we are done!!!" << std::endl;
 

    }


    //accepts and parses messages from a new connection, parsing its IP and port
    //since the only time a new message will appear on a socket is when there is 
    //a new client connecting, we can say that these will only be "hello" messages
    void accept_and_parse_message_(int server_fd, struct sockaddr* adr, socklen_t* addrlen) {
        char buffer[1024 * 2];
        const char* ip_prefix = "IP: 127.0.0.";

        int sock = accept(server_fd, adr, addrlen);
        client_sock[num_clients] = sock;
        assert(sock >= 0 ); // pinged by a client
        int valread = read(sock, buffer, 1024);

        if (strncmp(buffer, ip_prefix, 12) == 0) {
            std::cout << "got connection from: " << buffer << std::endl;
            //the format of the message received will be "IP: [ip_address] PORT: [port]"
            std::string message(buffer);
            int index_port = message.find("PORT: ");
            int ip_len = index_port - 4;
            std::string client_ip = message.substr(4, ip_len);
            std::string client_port = message.substr(index_port + 6, 6);

            client_ips->add(new String(client_ip.c_str()));
            client_ports->add(new String(client_port.c_str()));
            client_messages->add(new String(message.c_str()));
            num_clients++;

            //tells all of the other clients the ip address and the port of this new client
            send_update_msg_(message.c_str());
        }
    }

    //sends an update message to all the connected clients telling them 
    //the ip address and port of a new client
    void send_update_msg_(const char* update) {
        for(int i = 0; i < num_clients; i++) {
            send(client_sock[i], update, strlen(update), 0);  
        }
        std::cout << "sent update messages about new client to all current clients" << std::endl;  
    } 


    //listens for "done" messages from the client
    void listen_for_done_messages(){ 
        char buffer[1024 * 2] = {0};
        for (int i = 0; i < num_clients; i++) {
            if(do_poll_(client_sock[i], 200)) {
                int valread = read(client_sock[i], buffer, 1024); 
                if((strncmp(buffer, "done", 4)) == 0){
                    std::cout << "got a done message from client #" << i << std::endl;
                    if(clients_done->index_of(i) == -1 ) {clients_done->add(i); }
                    num_clients_done++;
                }
            }
        }
    }


};
// author: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
/** Represents a client class that contains information on its own ip address and a message
 * to send to the server. There is also a flag to indicate whether or not this client has
 * the ability to send a message
 */ 
class Client : public NetworkingDevice {
public:
    //ip and port of this client
    char* ip;
    char* port;

    //ip and port for the server
    char* sip;
    int sport;

    //message information
    int send_msg; //1 if there is a message to send, 0 if not
    char* msg_ip; //ip address to send the message to
    char* msg; //the message

    //info about other clients
    int num_clients;
    StringArray* client_ips;
    StringArray* client_ports;

    //info about my socket
    struct sockaddr_in my_adr;
    int my_addrlen = sizeof(my_adr);


    Client (char* ip_addr, char* port_num, char* server_ip, int server_port, int send_msg_flag, char* msg_ip_addr, char* msg_to_send) {
        ip = ip_addr;
        port = port_num;
        sip = server_ip;
        sport = server_port;
        send_msg = send_msg_flag;
        msg_ip = msg_ip_addr;
        msg = msg_to_send;
        num_clients = 0;
        client_ips = new StringArray();
        client_ports = new StringArray();

    }

    Client (char* ip_addr, char* server_ip, int server_port, int send_msg_flag, char* msg_ip_addr, char* msg_to_send) {
        ip = ip_addr;
        port = "8080";
        sip = server_ip;
        sport = server_port;
        send_msg = send_msg_flag;
        msg_ip = msg_ip_addr;
        msg = msg_to_send;
        num_clients = 0;
        client_ips = new StringArray();
        client_ports = new StringArray();

    }

    ~Client() {
        delete [] ip;
        delete [] port;
        delete [] sip;
        delete [] msg;
        delete [] msg_ip;
        delete client_ips;
        delete client_ports;
    }

    // To initialize the client and set up the socket to communicate with the server
    void run_client() {
        int errnum;

       // connect to server
       int sock1 = connect_to_someone_(sip, sport);
        
        //construct client hello message
        char hello[100];
        strcpy(hello, "IP: ");
        strcat(hello, ip);
        strcat(hello, " PORT: ");
        strcat(hello, port);
        send(sock1, hello, strlen(hello), 0);
    
        printf("sent my IP address and port to the server \n");   

        //bind to a socket to accept communications on 

        int client_fd = create_my_socket_((struct sockaddr_in*)&my_adr, (socklen_t*)&my_addrlen, std::stoi(port));

        //wait before sending messages out to ensure everyone had enough time to bind to the socket
        usleep(1500);
        bool getting_updates = true;

        //in a loop, check if you have updates or a teardown message from the server, send a message
        //if you have one to send and you know where to send it to, and check to see if you received 
        //any messages
        while(getting_updates) {
            getting_updates = get_updates_(sock1);
            if (send_msg) {
                send_msg_();
            }
            //this cannot be an else, because send_msg_() can change the value of send_msg to 0
            if (!send_msg) {
                send(sock1, "done", 4, 0);
                std::cout << "sent done message to server " << std::endl;
            }
            check_for_messages_(client_fd, (struct sockaddr*)&my_adr, (socklen_t*)&my_addrlen);
        } 

        //we have previously received the teardown message from the server, now reply 
        //to let it know you're ready to close all your sockets
        send(sock1, "OK", 2, 0);
        std::cout << "sent ok message to server, now closing things " << std::endl;

        //check for messages once more before closing sockets -- in case someone sent a message and we haven't gotten it yet
        usleep(6000);
        check_for_messages_(client_fd, (struct sockaddr*)&my_adr, (socklen_t*)&my_addrlen);

        //close all sockets
        //close my socket
        close(sock1);
        close(client_fd);
        std::cout << "everything is closed" << std::endl;

    }  



     //parses the ip from the char* message
    //if no ip in the message, it returns a nullptr
    int get_port_idx_(char* ip, StringArray* ips, StringArray* ports) {
        int index_of_ip = -1;
        for (int i = 0; i < ips->size(); i++) {
            if (ips->get(i)->equals(new String(ip))) {
                index_of_ip = i;
            }
        }

        return index_of_ip;
    }

  

    //adds clients given their information in the buffer
    void update_clients_(char* buffer) {
        std::string message(buffer);

        int index_port = message.find("PORT: ");
        int ip_len = index_port - 4;
        std::string client_ip = message.substr(4, ip_len - 1);
        std::string client_port = message.substr(index_port + 6, 5);
        
        client_ips->add(new String(client_ip.c_str()));
        client_ports->add(new String(client_port.c_str()));

        num_clients++;

    }

    //sends the message msg to the ip msg_ip if there is one to send
    void send_msg_() {
        int errnum;

        int index_msg_ip = get_port_idx_(msg_ip, client_ips, client_ports);

        //if the ip is already stored in the list, we can send the message, if not, we can't send the message so we have to wait
        if (index_msg_ip != -1) {
            char* msg_port = client_ports->get(index_msg_ip)->c_str();
            int sock2 = connect_to_someone_(msg_ip, std::stoi(msg_port));

            send(sock2, msg, strlen(msg), 0);
            std::cout << "sent message to the client " << std::endl;
            close(sock2);
            send_msg = 0;
        }
    }

    //checks for any message from the server 
    //returns true if the message we got was an update, false if it was a teardown
    bool get_updates_(int sock) {
        const char* ip_prefix = "IP: 127.0.0.";
        char buffer[1024 * 2] = {0}; 

        int valread = read(sock, buffer, 1024); 
        //check if this is an update message or a teardown message 
        if(strncmp(ip_prefix, buffer, 12) == 0) {
            std::cout << "updating my clients to include: " << std::endl;
            printf("%s\n",buffer);
            //parse the string and add these clients 
            update_clients_(buffer);
        }

        if((strncmp(buffer, "teardown", 8)) == 0){
                std::cout << "got a teardown message, time to initiate teardown" << std::endl;
                printf("%s\n",buffer );
                return false;
        }
        
    }

    //checks to see if there are any pending connections with messages 
    void check_for_messages_(int client_fd, struct sockaddr* adr, socklen_t* addrlen) {
        char buffer[1024 * 2 ] = {0};

            if(do_poll_(client_fd, 200)) {
                int sock3 = accept(client_fd, adr, addrlen);
                int valread = read(sock3, buffer, 1024);
                std::cout << "got a message from another client: ";
                printf("%s\n",buffer );
                close(sock3);
            }
    }

};