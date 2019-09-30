//
// Created by Matias on 26/09/2019.
//

#include "server_peer_thread.h"

void Peer::Start(CmdInterpreter *interpreter) {
    std::vector<Message> msgs;
    try {
        proxy_client.Send(
                Message(interpreter->GetFromConfig("newClient"), true));

        while (proxy_client.IsConnected() && !this->IsClosed()\
                    && profile.GetLastCommand() != EXIT_CMD){
            Message m = proxy_client.GetReply();
            msgs = interpreter->ExecuteCommand(profile, m.GetText());
            for_each(msgs.begin(), msgs.end(), [&](Message &m) {
                proxy_client.Send(m);
            });
        }
    } catch (std::runtime_error &e){
        std::cout <<"thread end "<< e.what()<< std::endl;
    }
    this->Close();
}

void Peer::Close() {
    Thread::Close();
    this->proxy_client.Disconnect();
}
