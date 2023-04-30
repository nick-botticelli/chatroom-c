#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "message.h"

#define CMD_DELIMITER " "

typedef enum {
    ACTION_NOTHING,
    ACTION_LEAVE,
    ACTION_SHUTDOWN
} CommandAction;

typedef struct {
    Message message;
    CommandAction action;
} CommandResult;



extern inline bool sendMessage(int sock, Message message);
extern inline bool isSocketConnected(int sock);
extern inline void connectSocket(Node *node);
extern inline bool handleCommand(Node *nodeList, char *input, CommandResult *cmdResultOut);
