#include "sender_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "debug.h"

void sendMessage(int sock, Message *message) {
    if (send(sock, &message, sizeof(message), 0) == -1) {
        perror("Error: Send failed");
        // exit(EXIT_FAILURE);
    }
}

Message *handleCommand(char *input) {
    char *firstToken = strtok(input, CMD_DELIMITER);

    printf("firstToken: %s\n", firstToken);

    if (strcasecmp(firstToken, "/JOIN") == 0) {
        // Handle joining
    }
    else if (strcasecmp(firstToken, "/LEAVE") == 0) {
        // Handle leaving
    }
    else if (strcasecmp(firstToken, "/SHUTDOWN") == 0) {
        char *secondToken = strtok(NULL, CMD_DELIMITER);

        if (strcasecmp(firstToken, "ALL") == 0) {
            // Handle shutdown all
        }
        else {
            // Handle shutdown
        }
    }
    else if (strcasecmp(firstToken, "/HELP") == 0) {
        puts("Commands:\n"
             "\t/join <chat room IP> - Join a chat room\n"
             "\t/leave - Leave a chat room\n"
             "\t/shutdown - Shut down the client\n"
             "\t/shutdown all - Shut down all clients\n"
             "\n"
             "\t<chat message> - The message you want to send to the chat room");
    }
    else {
        // Handle chat message
        debug("TODO: Send chat message: \"%s\"\n", input);
    }

    return NULL;
}
