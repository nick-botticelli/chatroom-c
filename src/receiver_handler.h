#pragma once

#include "message.h"

#define CMD_PROMPT "> "
#define ANSI_MOVE_CURSOR_DOWN "\033[1A"

extern inline void repositionCursor();
extern inline void handleClient(Message message);
extern inline bool receiveMessage(int sock, Message *messageOut);
