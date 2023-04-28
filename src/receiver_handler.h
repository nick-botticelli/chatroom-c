#pragma once

#include "message.h"

#define CMD_PROMPT "> "
#define ANSI_MOVE_CURSOR_DOWN "\033[1A"

void handleClient(Message *message);
Message *receiveMessage(int sock);
