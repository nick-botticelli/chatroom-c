#pragma once

#include <stdbool.h>

// #define PORT 0xCAFE // 51966



/**
 * @brief Represents a node in the chat room. The first entry (the head) represents the initial
 * server information if creating a chat room or the server information if connecting to an
 * existing chat room.
 */
typedef struct Node {
    char *ip;
    unsigned short port;
    char *username;
    int sock;
    bool connected;
    bool initialNode;
    struct Node *nextNode;
} Node;

typedef struct {
    Node *nodeList;
    Node *curNode;
} ReceiveHandlerData;



/**
 * @brief Create a Node safely (initializes the entire struct)
 * @param ip the ip string
 * @param port the 2-byte (16-bit) port
 * @param username the person's username string to use in chat messages
 * @param createSocket whether to create a new socket immediately or leave null
 * @param initialNode whether the created node is the initial host node
 */
extern Node *createNode(char *ip, short port, char *username, bool createSocket, bool initialNode);

extern void printNodeList(Node *nodeList);
extern Node *acceptNode(Node **nodeList);
extern void addNode(Node **nodeList, Node *node);
extern void removeNode(Node **nodeList, Node *node);
