#include "sender_handler.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "debug.h"
#include "message.h"

bool sendMessage(int sock, Message message) {
    size_t serializedMessageLen;
    uint8_t *serializedMessage = serializeMessage(message, &serializedMessageLen);

    debug_hexdump(serializedMessage, serializedMessageLen);

    if (send(sock, &message, sizeof(message), 0) == -1) {
        perror("Error: Send failed");
        return false;
    }

    return true;
}

CommandResult *handleCommand(char *input) {
    CommandResult *cmdResult = malloc(sizeof(*cmdResult));
    cmdResult->action = ACTION_NOTHING;

    char *tokenizedInput = strdup(input);
    char *firstToken = strtok(tokenizedInput, CMD_DELIMITER);

    if (strcasecmp(firstToken, "/JOIN") == 0) {
        // Handle joining
        debug("Handling join command\n");
        // TODO: Use username from properties file
        cmdResult->message = createJoinMessage("Username");
    }
    else if (strcasecmp(firstToken, "/LEAVE") == 0) {
        // Handle leaving
        debug("Handling leave command\n");
        cmdResult->message = createLeaveMessage();
        cmdResult->action = ACTION_LEAVE;
    }
    else if (strcasecmp(firstToken, "/SHUTDOWN") == 0) {
        char *secondToken = strtok(NULL, CMD_DELIMITER);

        cmdResult->action = ACTION_SHUTDOWN;

        if (secondToken == NULL) {
            // Handle shutdown
            debug("Handling shutdown command\n");
            cmdResult->message = createLeaveMessage();
        }
        else if (strcasecmp(secondToken, "ALL") == 0) {
            // Handle shutdown all
            debug("Handling shutdown all command\n");
            cmdResult->message = createShutdownAllMessage();
        }
        else {
            fprintf(stderr, "Unknown shutdown option \"%s\"!\n", secondToken);
            return NULL;
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
        return NULL;
    }
    else if (firstToken[0] == '/') {
        fprintf(stderr, "Unknown command!\n");
        return NULL;
    }
    else {
        // Handle chat message
        debug("Handling chat message \"%s\"\n", input);
        cmdResult->message = createNoteMessage(input); 
    }

    return cmdResult;
}
