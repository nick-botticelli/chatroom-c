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
    MessageType msgType = getMessageType(message.header);

    printf("Message {\n");
    printf("\theader:\n");
    printf("\t\ttype = %d\n", msgType);

    switch (msgType) {
        case MSG_JOIN:
            printf("\tname = %s\n", message.username);
            break;
        case MSG_ADD_MEMBER:
            break;
        case MSG_MEMBER_LIST:
            break;
        case MSG_NOTE:
            break;
        case MSG_LEAVE:
            break;
    }

    printf("}\n");
#endif
}

inline void loadProperties(Node *nodeOut) {
    Properties *properties = property_read_properties(CONFIG_PATH);

    char *ip = property_get_property(properties, "ip");
    short port = atoi(property_get_property(properties, "port"));
    char *username = property_get_property(properties, "username");

    createNode(ip, port, username, nodeOut);
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

            // Send message to all recipients; all Messages created by handleCommand should be sent
            // to all clients
            Node *curNode = nodeList;
            while (curNode != NULL) {
                sendMessage(curNode->sock, cmdResult.message);

                // Move to next node
                curNode = nodeList->nextNode;
            }

            if (cmdResult.action == ACTION_LEAVE || cmdResult.action == ACTION_SHUTDOWN)
                break;
        }
    }

    printf("Shutting down...\n");
    
    return NULL;
}

void *receive_handler(void *nodeRaw) {
    Node *node = (Node *) nodeRaw;

    while (true) {
        Message message;

        if (receiveMessage(node->sock, &message)) {
            debug("Successfully deserialized message!");

            handleClient(message);
        }
    }

    return NULL;
}

void *initial_receive_handler(void *nodeRaw) {
    Node *node = (Node *) nodeRaw;
    struct sockaddr_in server_address;

    // create unnamed network socket for server to listen on
    if ((node->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating initial server socket");
        exit(EXIT_FAILURE);
    }
    
    // name the socket (making sure the correct network byte ordering is observed)
    server_address.sin_family      = AF_INET;           // accept IP addresses
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // accept clients on any interface
    server_address.sin_port        = htons(node->port); // port to listen on
    
    // binding unnamed socket to a particular port
    if (bind(node->sock, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("Error binding initial server socket");
        exit(EXIT_FAILURE);
    }
    
    // listen for client connections (pending connections get put into a queue)
    if (listen(node->sock, 1) == -1) {
        perror("Error listening on initial server socket");
        exit(EXIT_FAILURE);
    }

    while (true) {
        Node newNode;
        acceptNode(&node, &newNode);

        debug("Accepted client!\n");

        pthread_t receive_thread;
        if (pthread_create(&receive_thread, NULL, receive_handler, (void *) &newNode) == -1) {
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
    
    Node node;
    loadProperties(&node);

    debug("IP: %s", node.ip);
    debug("Port: %d", node.port);
    debug("Username: %s", node.username);

    // Create sender thread
    if (pthread_create(&senderThread, NULL, send_handler, (void *) &node) < 0) {
        perror("Error: Could not create sender thread");
        result = EXIT_FAILURE;
        goto END;
    }

    if (node.ip == NULL) {
        // Create initial receiver thread
        if (pthread_create(&initialReceiverThread, NULL, initial_receive_handler, (void *) &node) < 0) {
            perror("Error: Could not create initial receiver thread");
            result = EXIT_FAILURE;
            goto END;
        }

        // Detach receiver thread
        if (pthread_detach(initialReceiverThread)) {
            fprintf(stderr, "Error: Could not join initial receiver thread!\n");
            return EXIT_FAILURE;
        }
    }

    // Join sender thread
    if (pthread_join(senderThread, NULL)) {
        fprintf(stderr, "Error: Could not join sender thread!\n");
        result = EXIT_FAILURE;
        goto END;
    }

END:
    return result;
}
