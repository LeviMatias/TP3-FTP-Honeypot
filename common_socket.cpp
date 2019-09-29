//
// Created by Matias on 27/09/2019.
//

#include "common_socket.h"

//TODO
// -Replace MSG_NOSIGNAL & SHUTDOWN_RD WITH NETDB CONSTANTS
// -throw exceptions instead of errors

Socket::Socket() :ai(false) {
    this->fd = 0;
    this->connected = NOT_CONNECTED;
}

Socket::Socket(std::string host, int service, bool is_passive) : ai(is_passive){
    this->fd = 0;
    this->connected = NOT_CONNECTED;
    std::string port = std::to_string(service);
    ai.s = getaddrinfo(host.c_str(),\
                        port.c_str(), \
                        &(this->ai.hints),
                        &this->ai.result);
    if (ai.s != 0) {
        throw std::runtime_error((std::string)gai_strerror(ai.s)\
        + "get addrinfo error" + LOCATION);
    }
    auto ptr = ai.result;
    this->fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
}

Socket::Socket(int my_fd, int connected_fd) : ai(false){
    this->fd = my_fd;
    this->connected = connected_fd;
}

void Socket::Connect() {
    int skt = this->fd;
    if (ai.s != 0) return;
    for (auto ptr = ai.result; ptr != nullptr && skt == 0; ptr = ptr->ai_next){
        if (skt > 0) {
            this->fd = skt;
            int s = connect(this->fd, ptr->ai_addr, ptr->ai_addrlen);
            if (s != -1) {
                this->connected = this->fd;
                return;
            }
        }
        skt = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    }
    close(this->fd);
    throw std::runtime_error((std::string)strerror(errno)\
                + "could not connect" + LOCATION);
}

void Socket::Shutdown() {
    if (this->fd != -1){
        int SHUT_RDWR = 2;
        if (this->connected != -1 && shutdown(this->connected, SHUT_RDWR) == -1){
            printf("Closing connection error: %s\n", strerror(errno));
        }
        if (this->connected != -1 && this->connected != this->fd){
            close(this->connected);
        }
        close(this->fd);
        this->fd = -1;
    }
}

void Socket::Send(std::vector<char> msg) {
    unsigned int sent = 0;
    int s = 0;
    bool is_the_socket_valid = true;
    while (sent < msg.size() && is_the_socket_valid) {
        int MSG_NOSIGNAL = 0;//todo remove
        s = send(this->connected, &msg[sent], msg.size() - sent, MSG_NOSIGNAL);

        if (s <= 0) {
            is_the_socket_valid = false;
        } else {
            sent += s;
        }
    }

    if (!is_the_socket_valid) {
        throw std::runtime_error((std::string)strerror(errno)+\
                                    " send error" + LOCATION);
    }
}

int Socket::Accept() {
    int peer_fd = accept(this->fd, nullptr, nullptr); //todo pass additional O_NONBLOCK
    if (peer_fd != -1){
        this->connected = peer_fd;
    }
    return peer_fd;
}

void Socket::BindAndListen() {
    int s = -1;
    const int val = 1; //configure socket to reuse address if TIME WAIT
    setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const char *>(&val), sizeof(val));

    for (auto ptr = ai.result; ptr != nullptr && s == -1; ptr = ptr->ai_next){
        s = bind(this->fd, ptr->ai_addr, ptr->ai_addrlen);
    }
    if (s != -1){
        s = listen(this->fd, 10);
    }
    if (s == -1){
        throw std::runtime_error((std::string)strerror(errno)+\
                                " bind error" + LOCATION );
    }
}

bool Socket::Receive1Byte(char* c){
    int s = 1;
    int r = 0;

    while (r < 1 && s > 0 && this->connected != -1) {
        s = recv(this->connected, c, 1, 0);
        if (s <= 0) { // there was an error
            throw std::runtime_error((std::string)strerror(errno)+\
            "rec error" + LOCATION);
        } else{
            r += s;
        }
    }
    return (s > 0);
}

bool Socket::IsConnected() {
    return this->connected != NOT_CONNECTED;
}

Socket::~Socket() {
    this->Shutdown();
}
