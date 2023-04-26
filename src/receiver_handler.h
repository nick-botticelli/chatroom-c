#pragma once

#include "message.h"

void handleClient(Message *message);
Message *receiveMessage(int sock);
