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


chatnode_properties load_properties() {

    char* properties_file = "chatnode.properties";
    Properties* properties;
    // char* value;

    chatnode_properties chatnode_props;

    chatnode_props.ip = property_get_property(properties, "ip");

    chatnode_props.port = property_get_property(properties, "port");

    chatnode_props.username = property_get_property(properties, "username");

    return chatnode_props;
    
    // printf("\nValue for %s: %s\n", key, value);
} 



void *send_handler(void *chatnode_props_raw) {
    chatnode_properties chatnode_props = *(chatnode_properties*) chatnode_props_raw;
    int sock;

    int32_t inputRaw, input;
    int32_t responseRaw, response;
    ssize_t response_length;

    struct sockaddr_in serv_addr;

    // TODO: Read properties file (chatnode.properties): read IP + port + username

    // Initialize socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Failed to create socket");
        return NULL;
    }

    // Command loop
    puts("Enter a command, or type '/help' for a list of commands.\n");
    while (true) {
        char cmdBuf[128]; // 100 characters for message + command

        printf("> ");

        // Read command input and craft outbound message
        scanf(" %127[^\n]", cmdBuf);
        // debug("Input: %s\n", cmdBuf);
        Message *outboundMsg = handleCommand(cmdBuf);

        // Send message
        // TODO: Loop through all members of chat room to send to
        // sendMessage(sock, outboundMsg);
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

void *receive_handler(void *unused) {
    int sock;

    int32_t inputRaw, input;
    int32_t responseRaw, response;
    ssize_t response_length;

    struct sockaddr_in serv_addr;

    // Initialize socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Failed to create socket");
        return NULL;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_ADDR, &serv_addr.sin_addr) <= 0) {
        perror("Error: Invalid address (address not supported)");
        return NULL;
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error: Failed to connect to server");
        return NULL;
    }

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
    puts("=== chat_node ===\n");

    pthread_t senderThread, receiverThread;


    chatnode_properties chatnode_props = load_properties();

    // Create sender thread
    if (pthread_create(&senderThread, NULL, send_handler, (void *) &chatnode_props) < 0) {
        perror("Error: Could not create sender thread");
        return EXIT_FAILURE;
    }

    // Create receiver thread
    // if (pthread_create(&receiverThread, NULL, receive_handler, (void *) NULL) < 0) {
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
