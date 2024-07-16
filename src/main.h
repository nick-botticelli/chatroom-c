#pragma once

#include <stdio.h>

#include "chat_node.h"
#include "properties.h"

#define CONFIG_PATH "chatnode.properties"

extern Node *loadProperties();
extern void *receive_handler(void *recHandlerDataRaw);
