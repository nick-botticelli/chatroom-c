#include "main.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include "chat_node.h"
#include "debug.h"
#include "properties.h"
#include "receiver_handler.h"
#include "sender_handler.h"

// TODO: Move this to debug.c
extern inline void debugMessage(Message message) {
#ifndef NDEBUG
    MessageType messageType = getMessageType(message.header);

    printf("Message {\n");
    printf("\theader:\n");
    printf("\t\ttype = %d\n", messageType);

    switch (messageType) {
        case MSG_JOIN:
            printf("\tusername = %s\n", message.username);
            printf("\tport = %d\n", message.port);
            break;
        case MSG_WELCOME:
            printf("\tremoteUsername = %s\n", message.remoteUsername);
        case MSG_ADD_MEMBER:
            printf("\tWIP\n");
            break;
        case MSG_NOTE:
            printf("\tnote = %s\n", message.note);
            break;
        case MSG_LEAVE:
            printf("\tshutdownAll = %s\n", getBit(message.header, 0) ? "true" : "false");
            break;
    }

    printf("}\n");
#endif
}

inline Node *loadProperties() {
    Properties *properties = property_read_properties(CONFIG_PATH);

    char *ip = property_get_property(properties, "ip");
    short port = atoi(property_get_property(properties, "port"));
    char *username = property_get_property(properties, "username");

    return createNode(ip, port, username, true, ip == NULL);
}

void *send_handler(void *nodeListRaw) {
    Node *nodeList = (Node *) nodeListRaw;

    // Command loop
    puts("Enter a command, or type '/help' for a list of commands.\n");

    while (true) {
        char cmdBuf[MAX_PAYLOAD_SIZE];

        printf(CMD_PROMPT);

        // Read terminal input (command) and craft outbound message
        scanf(" %126[^\n]", cmdBuf); // MAX_PAYLOAD_SIZE - 1 = 126  ->  1 byte saved for null terminator
        // debug("Input: %s\n", cmdBuf);
        CommandResult cmdResult;
        bool handleCmdSuccess = handleCommand(nodeList, cmdBuf, &cmdResult);

        if (handleCmdSuccess) {
            debugMessage(cmdResult.message);

            debug("Sending to all...");
            sendMessageAll(nodeList, NULL, cmdResult.message);

            debug("Command result action: %d", cmdResult.action);

            if (cmdResult.action == ACTION_LEAVE || cmdResult.action == ACTION_SHUTDOWN)
                break;
        }
    }

    printf("Shutting down...\n");
    
    return NULL;
}

void *receive_handler(void *recHandlerDataRaw) {
    ReceiveHandlerData *recHandlerData = (ReceiveHandlerData *) recHandlerDataRaw;

    printNodeList(recHandlerData->nodeList);

    while (true) {
        Message message;

        if (!recHandlerData->curNode->connected) {
            // If no longer connected, remove the node and exit thread
            debug("Removing node!");
            removeNode(&recHandlerData->nodeList, recHandlerData->curNode);
            break;
        }

        if (receiveMessage(recHandlerData->nodeList, recHandlerData->curNode, &message)) {
            if (!handleClient(recHandlerData->nodeList, recHandlerData->curNode, message))
                break;
        }
    }

    free(recHandlerData);

    return NULL;
}

void *initial_receive_handler(void *nodeRaw) {
    Node *node = (Node *) nodeRaw;
    struct sockaddr_in server_address;

    // Create unnamed network socket for server to listen on
    if ((node->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating initial server socket");
        exit(EXIT_FAILURE);
    }
    
    // name the socket (making sure the correct network byte ordering is observed)
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    // Automatically generate port if not the initial host
    server_address.sin_port = node->initialNode ? htons(node->port) : 0;
    
    // Bind socket to a port
    if (bind(node->sock, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("Error binding initial server socket");
        exit(EXIT_FAILURE);
    }

    // Set port if needed
    if (!node->initialNode) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        getsockname(node->sock, (struct sockaddr *) &clientAddr, &clientAddrSize);
        node->port = ntohs(clientAddr.sin_port);
        debug("node->port: %d", node->port);
    }
    
    // Listen for client connections (pending connections get put into a queue)
    if (listen(node->sock, 1) == -1) {
        perror("Error listening on initial server socket");
        exit(EXIT_FAILURE);
    }

    while (true) {
        ReceiveHandlerData *recHandlerData = malloc(sizeof(ReceiveHandlerData));
        recHandlerData->nodeList = node;
        recHandlerData->curNode = acceptNode(&node);

        debug("Accepted client!\n");

        pthread_t receive_thread;
        if (pthread_create(&receive_thread, NULL, receive_handler, (void *) recHandlerData) == -1) {
            perror("Error: Could not create receiver thread");
            exit(EXIT_FAILURE);
        }
        
        // detach the thread so that we don't have to wait (join) with it to reclaim memory.
        // memory will be reclaimed when the thread finishes.
        if (pthread_detach(receive_thread) == -1) {
            perror("Error: Could not detach receiver thread");
            exit(EXIT_FAILURE);
        }
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
    int result = EXIT_SUCCESS;
    pthread_t senderThread, initialReceiverThread;

    puts("=== chat_node ===\n");
    
    Node *nodeList;
    Node *node = loadProperties();

    debug("IP: %s", node->ip);
    debug("Port: %d", node->port);
    debug("Username: %s", node->username);

    // Set the head of the node list to own server info
    if (node->initialNode) {
        nodeList = node;
    }
    else {
        nodeList = createNode(NULL, 0, node->username, false, false);
        nodeList->nextNode = node;
        node->username = NULL;
    }

    // Create sender thread
    if (pthread_create(&senderThread, NULL, send_handler, (void *) nodeList) < 0) {
        perror("Error: Could not create sender thread");
        result = EXIT_FAILURE;
        goto END;
    }

    // Create initial receiver thread
    if (pthread_create(&initialReceiverThread, NULL, initial_receive_handler, (void *) nodeList) < 0) {
        perror("Error: Could not create initial receiver thread");
        result = EXIT_FAILURE;
        goto END;
    }

    // Detach receiver thread
    if (pthread_detach(initialReceiverThread)) {
        fprintf(stderr, "Error: Could not join initial receiver thread!\n");
        return EXIT_FAILURE;
    }

    // Join sender thread
    if (pthread_join(senderThread, NULL)) {
        fprintf(stderr, "Error: Could not join sender thread!\n");
        result = EXIT_FAILURE;
        goto END;
    }

    debug("Final exit.");

END:
    return result;
}
