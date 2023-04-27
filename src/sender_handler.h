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



bool sendMessage(int sock, Message message);
CommandResult *handleCommand(char *input);
