#include "chat_node.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h> // TODO: Remove
#include <sys/socket.h>
#include <unistd.h>

#include "debug.h"

inline void printNodeList(Node *nodeList) {
#ifndef NDEBUG
    Node *curNode = nodeList;

    // Traverse linked list until we find the node to remove
    printf("===== Node list =====\n");
    while (curNode != NULL) {
        printf("\t%p: %s:%d - %s - \t -> %p\n", curNode, curNode->ip, curNode->port, curNode->username, curNode->nextNode);
        curNode = curNode->nextNode;
        sleep(1);
    }
    printf("=====================\n");
#endif
}

inline Node *createNode(char *ip, short port, char *username, bool createSocket) {
    Node *node = malloc(sizeof(Node));
    node->ip = ip;
    node->port = port;
    node->username = username;
    node->sock = createSocket ? socket(AF_INET, SOCK_STREAM, 0) : 0;
    node->connected = false;
    node->nextNode = NULL;

    return node;
}

inline Node *acceptNode(Node **nodeList) {
    struct sockaddr addr;
    socklen_t addrlen = sizeof(addr);
    int sock = accept((*nodeList)->sock, &addr, &addrlen);
    struct sockaddr_in *ipV4Addr = (struct sockaddr_in *) &addr;
    struct in_addr ipAddr = ipV4Addr->sin_addr;
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipAddr, ipStr, INET_ADDRSTRLEN);
    short port = ntohs(ipV4Addr->sin_port);

    Node *node = createNode(ipStr, port, NULL, false);
    node->sock = sock;
    node->connected = true;

    addNode(nodeList, node);
    return node;
}

inline void addNode(Node **nodeList, Node *node) {
    if (*nodeList == NULL) { // TODO
        *nodeList = node;
        // node->nextNode = NULL;
        return;
    }

    Node *curNode = *nodeList;
    while (curNode->nextNode != NULL) {
        // Move to next node
        curNode = curNode->nextNode;
    }

    debug("curNode: %p", curNode);
    debug("node: %p", node);

    curNode->nextNode = node;

    printNodeList(*nodeList);
}

inline void removeNode(Node **nodeList, Node *node) {
    Node *prevNode = NULL;
    Node *curNode = *nodeList;

    // Traverse linked list until we find the node to remove
    while (curNode != NULL && curNode != node) {
        prevNode = curNode;
        curNode = curNode->nextNode;
    }

    if (prevNode != NULL) {
        // curNode is the node to remove
        prevNode->nextNode = node;
        node->nextNode = node->nextNode;
    }
    else {
        // nodeList->node = node;
        *nodeList = node;

        if (curNode == *nodeList) {
            // Linked list is empty
            (*nodeList)->nextNode = NULL;
        }
        else {
            // First item is the node to remove (curNode)
            (*nodeList)->nextNode = curNode->nextNode;
        }
    }

    printNodeList(*nodeList);
}
