#include "receiver_handler.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // TODO: Remove

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
    MessageType messageType = getMessageType(message.header);

    // repositionCursor();

    switch (messageType) {
        case MSG_JOIN:
            // Handle join
            debug("Join received!");

            break;
        case MSG_ADD_MEMBER:
            // Handle add member
            debug("Add member received!");
            break;
        case MSG_MEMBER_LIST:
            // Handle member list
            debug("Member list received!");
            break;
        case MSG_NOTE:
            // Handle note
            debug("Note received!");
            break;
        case MSG_LEAVE:
            // Handle leave
            debug("Leave received!");
            break;
    }
}

inline bool receiveMessage(int sock, Message *messageOut) {
    uint8_t messageRaw[MAX_PAYLOAD_SIZE + 1];
    ssize_t messageRawLen = recv(sock, &messageRaw, sizeof(messageRaw), 0);

    if (messageRawLen == -1) {
        debug("Error reading message?");
        // sleep(1); // TODO: Remove
        return false;
    }

    debug("Received message hexdump:");
    debug_hexdump(messageRaw, messageRawLen);

    // Create message from received data
    deserializeMessage(messageRaw, messageOut);
    return true;
}
