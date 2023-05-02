#pragma once

#include "message.h"

// #define CMD_PROMPT "> "
#define CMD_PROMPT ""
#define ANSI_MOVE_CURSOR_DOWN "\033[1A"

extern inline void removePrompt();
extern inline void repositionCursor();
extern inline bool handleClient(Node *nodeList, Node *node, Message message);
extern inline bool receiveMessage(Node *nodeList, Node *node, Message *messageOut);
