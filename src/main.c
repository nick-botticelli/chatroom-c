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
#include "receiver_handler.h"
#include "sender_handler.h"
#include "properties.h"

// TODO: Remove
#define SERVER_ADDR "127.0.0.1"

// TODO: Move this to debug.c
void debugMessage(Message message) {
    MessageType msgType = getMessageType(message.header);

    debug("Message {\n");
    debug("\theader:\n");
    debug("\t\ttype = %d\n", msgType);

    switch (msgType) {
        case MSG_JOIN:
            debug("\tname = %s\n", message.name);
            break;
        case MSG_ADD_MEMBER:
            break;
        case MSG_MEMBER_LIST:
            break;
        case MSG_NOTE:
            break;
        case MSG_LEAVE:
            break;
        case MSG_SHUTDOWN_ALL:
            break;
    }

    debug("}\n");
}

chatnode_properties load_properties() {
    char* properties_file = "chatnode.properties";
    Properties* properties;
    chatnode_properties chatnode_props;

    chatnode_props.ip = property_get_property(properties, "ip");
    chatnode_props.port = property_get_property(properties, "port");
    chatnode_props.username = property_get_property(properties, "username");

    return chatnode_props;
}

void *send_handler(void *ioDataRaw) {
    int32_t inputRaw, input;
    int32_t responseRaw, response;
    ssize_t response_length;
    struct sockaddr_in serv_addr;
    IOData ioData = *(IOData *) ioDataRaw;

    // TODO: Read properties file (chatnode.properties): read IP + port + username

    // Command loop
    puts("Enter a command, or type '/help' for a list of commands.\n");
    while (true) {
        char cmdBuf[128]; // 100 characters for message + command

        printf(CMD_PROMPT);

        // Read command input and craft outbound message
        scanf(" %127[^\n]", cmdBuf);
        // debug("Input: %s\n", cmdBuf);
        CommandResult *cmdResult = handleCommand(cmdBuf);

        if (cmdResult != NULL) {
            // debugMessage(cmdResult->message);

            // Send message
            // TODO: Loop through all members of chat room to send to
            sendMessage(ioData.sock, cmdResult->message);
        }
    }

    // sock = socket(AF_INET, SOCK_STREAM, 0);
    // if (sock < 0) {
    //     perror("Failed to create socket");
    //     return NULL;
    // }

    // bzero((char *) &serv_addr, sizeof(serv_addr));

    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_port = htons(PORT);

    // if (inet_pton(AF_INET, SERVER_ADDR, &serv_addr.sin_addr) <= 0) {
    //     perror("Error: Invalid address (address not supported)");
    //     return NULL;
    // }

    // if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    //     perror("Error: Failed to connect to server");
    //     return NULL;
    // }

    close(sock);
    return NULL;
}

void *receive_handler(void *ioDataRaw) {
    int sock;

    int32_t inputRaw, input;
    int32_t responseRaw, response;
    ssize_t response_length;

    struct sockaddr_in serv_addr;

    IOData ioData = *(IOData *) ioDataRaw;

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // if (inet_pton(AF_INET, SERVER_ADDR, &serv_addr.sin_addr) <= 0) {
    //     perror("Error: Invalid address (address not supported)");
    //     return NULL;
    // }

    // if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    //     perror("Error: Failed to connect to server");
    //     return NULL;
    // }

    printf("Client connected successfully.\n");
    while (true) {
        Message *message = receiveMessage(sock);
        handleClient(message);

        // printf("Server response: %d\n", message->);
    }

    close(sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t senderThread, receiverThread;

    puts("=== chat_node ===\n");
    
    chatnode_properties chatnode_props = load_properties();
    
    Node node;
    node.ip = chatnode_props.ip;
    node.port = atoi(chatnode_props.port);
    node.name = chatnode_props.username;

    IOData ioData;
    ioData.node = node;
    ioData.sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Failed to create socket");
        return EXIT_FAILURE;
    }

    // Create sender thread
    if (pthread_create(&senderThread, NULL, send_handler, (void *) &ioData) < 0) {
        perror("Error: Could not create sender thread");
        return EXIT_FAILURE;
    }

    // Create receiver thread
    // if (pthread_create(&receiverThread, NULL, receive_handler, (void *) &ioData) < 0) {
    //     perror("Error: Could not create receiver thread");
    //     return EXIT_FAILURE;
    // }

    // Join sender thread
    if (pthread_join(senderThread, NULL)) {
        fprintf(stderr, "Error: Could not join sender thread!\n");
        return EXIT_FAILURE;
    }

    // Join receiver thread
    // if (pthread_join(receiverThread, NULL)) {
    //     fprintf(stderr, "Error: Could not join receiver thread!\n");
    //     return EXIT_FAILURE;
    // }

    return EXIT_SUCCESS;
}
