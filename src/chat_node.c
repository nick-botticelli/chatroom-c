#include "chat_node.h"

#include <stdio.h> // TODO: Remove
#include <stdlib.h>
#include <sys/socket.h>

inline void createNode(char *ip, short port, char *username, Node *nodeOut) {
    nodeOut->ip = ip;
    nodeOut->port = port;
    nodeOut->username = username;
    nodeOut->sock = socket(AF_INET, SOCK_STREAM, 0);
    nodeOut->connected = false;
    nodeOut->nextNode = NULL;
}

inline void acceptNode(Node **nodeList, Node *nodeOut) {
    nodeOut->ip = NULL;
    nodeOut->port = 0;
    nodeOut->username = NULL;
    nodeOut->sock = accept((*nodeList)->sock, NULL, NULL);
    nodeOut->connected = true;
    nodeOut->nextNode = NULL;

    addNode(nodeList, nodeOut);
}

inline void addNode(Node **nodeList, Node *node) {
    if (nodeList == NULL) {
        *nodeList = node;
        node->nextNode = NULL;
        return;
    }

    while ((*nodeList)->nextNode != NULL) {
        // Move to next node
        *nodeList = (*nodeList)->nextNode;
    }

    (*nodeList)->nextNode = node;
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
}
