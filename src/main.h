#pragma once

#include "chat_node.h"
#include "properties.h"

// TODO: Remove
#define SERVER_ADDR "127.0.0.1"

#define CONFIG_PATH "chatnode.properties"

extern inline void loadProperties(Node *nodeOut);
