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



extern bool sendMessage(int sock, Message message);
extern void sendMessageAll(Node *nodeList, Node *nodeExclude, Message message);
extern void connectSocket(Node *nodeList, Node *node);
extern bool connectToNode(Node *nodeList, Node *node, bool newJoin, Message *joinMessageOut);
extern bool handleCommand(Node *nodeList, char *input, CommandResult *cmdResultOut);
