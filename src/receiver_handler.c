#include "receiver_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "debug.h"

void repositionCursor() {
    // Remove the two command prompt characters ('> '),
    // then move input cursor down,
    // then print command prompt characters
    debug("Reposition cursor!");
    printf("\b\b%s" CMD_PROMPT, ANSI_MOVE_CURSOR_DOWN);

    // Move input cursor down
    // printf("%s", ANSI_MOVE_CURSOR_DOWN);
}

void handleClient(Message *message) {
    int messageType = message->header >> 5;

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

Message *receiveMessage(int sock) {
    Message message;

    ssize_t response_length = recv(sock, &message, sizeof(message), 0);
    if (response_length == -1) {
        perror("Error: Receive failed");
        return NULL;
    }

    // Create message from received data
    return NULL;
}
