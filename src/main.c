#include <stdio.h>
#include <stdlib.h>

#include "chat_node.h"

void *connection_handler(void *threadid)
{
    int thread_num = (int) threadid;
    int sock_desc;

    int32_t inputRaw, input;
    int32_t responseRaw, response;
    ssize_t response_length;

    struct sockaddr_in serv_addr;

    sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_desc < 0) {
        perror("Failed to create socket!");
        return EXIT_FAILURE;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_ADDR, &serv_addr.sin_addr) <= 0) {
        perror("Error: Invalid address (address not supported)!");
        return EXIT_FAILURE;
    }

    if (connect(sock_desc, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error: Failed to connect to server!");
        return EXIT_FAILURE;
    }

    printf("Client connected successfully: %d\n", thread_num);
    while (true) {
        printf("For thread: %d\n", thread_num);

        printf("Enter input: ");
        scanf("%d", &inputRaw);

        input = htonl(inputRaw);

        if (send(sock_desc, &input, sizeof(int32_t), 0) == -1) {
            perror("Error: Send failed!");
            return EXIT_FAILURE;
        }

        response_length = recv(sock_desc, &responseRaw, sizeof(int32_t), 0);
        if (response_length != sizeof(int32_t)) {
            perror("Error: Receive failed!");
            return EXIT_FAILURE;
        }

        response = ntohl(responseRaw);

        printf("Server response: %d\n", response);
        
        sleep(5);
    }

    close(sock_desc);
    return EXIT_SUCCESS;
}


int main(int argc, char *argv[]) {
    puts("=== chat_node ===\n");

    return EXIT_SUCCESS;
}
