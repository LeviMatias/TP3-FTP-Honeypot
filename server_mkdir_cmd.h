//
// Created by Matias on 28/09/2019.
//

#ifndef TP3_SERVER_MKDIR_CMD_H
#define TP3_SERVER_MKDIR_CMD_H

#include "server_command.h"

class MakeDirCmd : public Command{
public:
    virtual std::vector<Message> Execute(SafeSet<std::string> &set,\
                                        std::string args);
};


#endif //TP3_SERVER_MKDIR_CMD_H