#include "receiver_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "debug.h"

inline void repositionCursor() {
    // Remove the two command prompt characters ('> '),
    // then move input cursor down,
    // then print command prompt characters
    printf("\b\b%s" CMD_PROMPT, ANSI_MOVE_CURSOR_DOWN);

    // Move input cursor down
    // printf("%s", ANSI_MOVE_CURSOR_DOWN);
}

inline void handleClient(Message message) {
    int messageType = message.header >> 5;

    repositionCursor();

    switch (messageType) {
        case 0:
            // Handle join
            debug("Join!");
            break;
        case 1:
            // Handle add member
            debug("Add member!");
            break;
        case 2:
            // Handle member list
            debug("Join!");
            break;
        case 3:
            // Handle message
            debug("Message!");
            break;
        case 4:
            // Handle leave
            debug("Leave!");
            break;
        case 5:
            // Handle shutdown all
            debug("Shutdown all!");
            break;
    }
}

inline bool receiveMessage(int sock, Message *messageOut) {
    uint8_t messageRaw[MAX_PAYLOAD_SIZE + 1];
    ssize_t messageRawLen = recv(sock, &messageRaw, sizeof(messageRaw), 0);

    if (messageRawLen == -1) {
        debug("Error reading message?");
        return false;
    }

    debug("Received message hexdump:");
    debug_hexdump(messageRaw, messageRawLen);

    // Create message from received data
    deserializeMessage(messageRaw, messageOut);
    return true;
}
