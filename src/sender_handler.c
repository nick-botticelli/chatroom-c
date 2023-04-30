#include "sender_handler.h"

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "debug.h"
#include "message.h"

inline bool sendMessage(int sock, Message message) {
    bool result = true;
    size_t serializedMessageLen;
    uint8_t *serializedMessage = serializeMessage(message, &serializedMessageLen);

    debug_hexdump(serializedMessage, serializedMessageLen);

    if (send(sock, serializedMessage, serializedMessageLen, 0) == -1) {
        perror("Error: Send failed");
        result = false;
    }

    free(serializedMessage);
    return result;
}

inline bool isSocketConnected(int sock) {
    int err = 0;
    socklen_t len = sizeof(err);
    int result = getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len);

    if (result == -1) {
        perror("Error: Could not check the socket status");
        return false;
    }

    printf("err: %d, %d\n", err, result);
    
    return err == 0;
}

inline void connectSocket(Node *node) {
    struct sockaddr_in serv_addr;

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(node->port);

    if (inet_pton(AF_INET, node->ip, &serv_addr.sin_addr) <= 0) {
        perror("Error: Invalid address (address not supported)");
        return;
    }

    if (connect(node->sock, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) == -1) {
        perror("Error: Failed to connect to server");
        return;
    }

    node->connected = true;
}

inline bool handleCommand(Node *nodeList, char *input, CommandResult *cmdResultOut) {
    cmdResultOut->action = ACTION_NOTHING;

    char *tokenizedInput = strdup(input);
    char *firstToken = strtok(tokenizedInput, CMD_DELIMITER);

    if (strcasecmp(firstToken, "/JOIN") == 0) {
        debug("Handling join command\n");

        if (nodeList->ip == NULL) {
            fprintf(stderr, "No server specified to join; you are currently the initial host!\n");
            return false;
        }
        else if (nodeList->connected) {
            fprintf(stderr, "You are already connected to the chat room!\n");
            return false;
        }
        else {
            // Join the first Node in the NodeList
            connectSocket(nodeList);

            if (nodeList->connected)
                cmdResultOut->message = createJoinMessage(nodeList->username);
            else
                return false;
        }
    }
    else if (strcasecmp(firstToken, "/LEAVE") == 0) {
        debug("Handling leave command\n");
        cmdResultOut->message = createLeaveMessage(false);
        cmdResultOut->action = ACTION_LEAVE;
    }
    else if (strcasecmp(firstToken, "/SHUTDOWN") == 0) {
        char *secondToken = strtok(NULL, CMD_DELIMITER);

        cmdResultOut->action = ACTION_SHUTDOWN;

        if (secondToken == NULL) {
            debug("Handling shutdown command\n");
            cmdResultOut->message = createLeaveMessage(false);
        }
        else if (strcasecmp(secondToken, "ALL") == 0) {
            debug("Handling shutdown all command\n");
            cmdResultOut->message = createLeaveMessage(true);
        }
        else {
            fprintf(stderr, "Unknown shutdown option \"%s\"!\n", secondToken);
            return false;
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
        return false;
    }
    else if (firstToken[0] == '/') {
        fprintf(stderr, "Unknown command!\n");
        return false;
    }
    else {
        debug("Handling chat message \"%s\"\n", input);
        cmdResultOut->message = createNoteMessage(input); 
    }

    return true;
}
