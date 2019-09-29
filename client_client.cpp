//
// Created by Matias on 26/09/2019.
//

#include "client_client.h"
#include "server_command_interpreter.h"

void Client::Start(const std::string host,const int port) {
    try {
        this->server.Connect(host, port);
    } catch(const std::runtime_error &e) {
       std::cout << e.what() << std::endl;
    }

    bool msg_end = false;
    std::string line;
    try {
        while (server.IsConnected()) {
            while (!msg_end) {
                Message msg = server.GetReply();
                std::cout << msg;
                msg_end = msg.IsLastMesssage();
            }
            getline(std::cin>> std::ws, line);
            Message msg = Message(line, true);
            server.Send(msg);
            msg_end = false;
        }
    } catch(std::runtime_error &e) {
        std::cout << e.what() << std::endl;
    }
}