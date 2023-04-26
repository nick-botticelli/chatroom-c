#pragma once

#include <stdlib.h>

#include "message.h"

#define CMD_DELIMITER " "

void sendMessage(int sock, Message *message);
Message *handleCommand(char *input);
