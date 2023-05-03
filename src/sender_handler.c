#include "sender_handler.h"

#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "debug.h"
#include "main.h"
#include "message.h"

inline bool sendMessage(int sock, Message message) {
    bool result = true;
    size_t serializedMessageLen;
    uint8_t *serializedMessage = serializeMessage(message, &serializedMessageLen);

    debug("Message send hexdump:");
    debug_hexdump(serializedMessage, serializedMessageLen);

    if (send(sock, serializedMessage, serializedMessageLen, 0) == -1) {
        perror("Error: Send failed");
        result = false;
    }

    free(serializedMessage);
    return result;
}

inline void sendMessageAll(Node *nodeList, Node *nodeExclude, Message message) {
    printNodeList(nodeList);
    Node *curNode = nodeList;
    while (curNode != NULL) {
        // Do not send to nodes (including self) with null IPs
        if (curNode->ip != NULL && curNode != nodeExclude)
            sendMessage(curNode->sock, message);

        curNode = curNode->nextNode;
    }
}

inline void connectSocket(Node *nodeList, Node *node) {
    struct sockaddr_in serv_addr;

    bzero((char *) &serv_addr, sizeof(serv_addr));

    debug("connectSocket(): %s:%d", node->ip, node->port);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(node->port);

    if (inet_pton(AF_INET, node->ip, &serv_addr.sin_addr) <= 0) {
        perror("Error: Invalid address (address not supported)");
        return;
    }

    if (connect(node->sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
        // TODO: This is getting stuck here when first attempt to connect fails
        // TODO: This is also failing with "Connection reset by peer" when 4th person connects;
        //       this may be caused by the fact connectSocket is being double called with same port?
        //       From Person2 perspective, Person3 isn't connected either.
        perror("Error: Failed to connect to node");
        return;
    }

    node->connected = true;

    pthread_t sr_receive_thread;
    ReceiveHandlerData *recHandlerData = malloc(sizeof(ReceiveHandlerData));
    recHandlerData->nodeList = nodeList;
    recHandlerData->curNode = node;
    if (pthread_create(&sr_receive_thread, NULL, receive_handler, (void *) recHandlerData) == -1) {
        perror("Error: Could not create socket-reuse receiver thread");
    }
    
    // Detach the thread so that we don't have to wait (join) with it to reclaim memory; memory
    // will be reclaimed when the thread finishes.
    if (pthread_detach(sr_receive_thread) == -1) {
        perror("Error: Could not detach socket-reuse receiver thread");
    }
}

inline bool connectToNode(Node *nodeList, Node *node, bool newJoin, Message *joinMessageOut) {
    // Join the first Node in the NodeList
    connectSocket(nodeList, node);

    if (node->connected) {
        addNode(&nodeList, node);
        debug("connectToNode(): new join: %d", newJoin);
        *joinMessageOut = createJoinMessage(nodeList->username, nodeList->port, newJoin);
        return true;
    }
    
    return false;
}

inline bool handleCommand(Node *nodeList, char *input, CommandResult *cmdResultOut) {
    cmdResultOut->action = ACTION_NOTHING;

    char *tokenizedInput = strdup(input);
    char *firstToken = strtok(tokenizedInput, CMD_DELIMITER);

    if (strcasecmp(firstToken, "/JOIN") == 0) {
        debug("Handling join command\n");

        if (nodeList->initialNode) {
            fprintf(stderr, "No server specified to join; you are currently the initial host!\n");
            return false;
        }
        else if (nodeList->nextNode->connected) {
            fprintf(stderr, "You are already connected to the chat room!\n");
            return false;
        }
        else {
            if (connectToNode(nodeList, nodeList->nextNode, true, &cmdResultOut->message))
                printf("You successfully the chat room.\n");
            else
                return false;
        }
    }
    else if (strcasecmp(firstToken, "/LEAVE") == 0) {
        debug("Handling leave command\n");
        cmdResultOut->message = createLeaveMessage(false);
        cmdResultOut->action = ACTION_LEAVE;
        printf("You are now leaving the chat room.\n");
    }
    else if (strcasecmp(firstToken, "/SHUTDOWN") == 0) {
        char *secondToken = strtok(NULL, CMD_DELIMITER);

        cmdResultOut->action = ACTION_SHUTDOWN;

        if (secondToken == NULL) {
            debug("Handling shutdown command\n");
            cmdResultOut->message = createLeaveMessage(false);
            printf("You are now leaving the chat room and shutting down.\n");
        }
        else if (strcasecmp(secondToken, "ALL") == 0) {
            debug("Handling shutdown all command\n");
            cmdResultOut->message = createLeaveMessage(true);
            printf("You are now shutting the chat room down.\n");
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
        debug("Handling chat message \"%s\"", input);
        cmdResultOut->message = createNoteMessage(input);
        printf("%s> %s\n", nodeList->username, input);
    }

    return true;
}
