#include "chat_node.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include "debug.h"
#include "main.h"

Node *createNode(char *ip, short port, char *username, bool createSocket, bool initialNode) {
    Node *node = malloc(sizeof(Node));
    node->ip = ip;
    node->port = port;
    node->username = username;
    node->sock = createSocket ? socket(AF_INET, SOCK_STREAM, 0) : 0;
    node->connected = false;
    node->initialNode = initialNode;
    node->nextNode = NULL;

    return node;
}

Node *acceptNode(Node **nodeList) {
    struct sockaddr addr;
    socklen_t addrlen = sizeof(addr);
    int sock = accept((*nodeList)->sock, &addr, &addrlen);
    struct sockaddr_in *ipV4Addr = (struct sockaddr_in *) &addr;
    struct in_addr ipAddr = ipV4Addr->sin_addr;
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipAddr, ipStr, INET_ADDRSTRLEN);
    char *ip = malloc(strlen(ipStr) + 1);
    strcpy(ip, ipStr);
    // short port = ntohs(ipV4Addr->sin_port);

    Node *node = createNode(ip, 0, NULL, false, false);
    node->sock = sock;
    node->connected = true;

    addNode(nodeList, node);
    return node;
}

void addNode(Node **nodeList, Node *node) {
    if (*nodeList == NULL) { // TODO
        *nodeList = node;
        // node->nextNode = NULL;
        return;
    }

    Node *curNode = *nodeList;
    while (curNode->nextNode != NULL) {
        // Move to next node
        curNode = curNode->nextNode;

        // If node is already in the node list, do nothing
        if (curNode == node)
            return;
    }

    debug("curNode: %p", curNode);
    debug("node: %p", node);

    curNode->nextNode = node;

    printNodeList(*nodeList);
}

void removeNode(Node **nodeList, Node *node) {
    Node *prevNode = NULL;
    Node *curNode = *nodeList;

    debug("Node to remove: %p (%s)", node, node->username);

    // Traverse linked list until we find the node to remove
    while (curNode != NULL && curNode != node) {
        prevNode = curNode;
        curNode = curNode->nextNode;
    }

    if (curNode == NULL) {
        debug("Could not find node %p (%s) to remove in node list!", node, node->username);
        return;
    }

    debug("Found node to remove: %p (%s)", curNode, curNode->username);

    if (prevNode != NULL) {
        // curNode is the node to remove and not the head of the linked list
        // prevNode->nextNode = node;
        // node->nextNode = node->nextNode;
        prevNode->nextNode = curNode->nextNode;
        debug("Normal node removal!");
    }
    else {
        // curNode is the head of the linked list
        // nodeList->node = node;
        *nodeList = node;

        debug("Removing the head in node list!");
        *nodeList = curNode->nextNode;
    }

    // Clean up old node
    curNode->connected = false;
    free(curNode->ip);
    free(curNode->username);
    close(curNode->sock);
    free(curNode);

    printNodeList(*nodeList);
}
