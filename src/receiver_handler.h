#pragma once

#include "message.h"

// #define CMD_PROMPT "> "
#define CMD_PROMPT ""
#define ANSI_MOVE_CURSOR_DOWN "\033[1A"

extern void removePrompt();
extern void repositionCursor();
extern bool handleClient(Node *nodeList, Node *node, Message message);
extern bool receiveMessage(Node *nodeList, Node *node, Message *messageOut);
