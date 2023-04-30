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
    struct Node *nextNode;
} Node;



/**
 * @brief Create a Node linked list safely (initializes the entire struct) with an initial
 *        Node
 * @param nodeListOut the output Node linked list location
 * @param initialNode the initial node to put in the new NodeList
 */
// extern inline void createNodeList(Node *nodeListOut, Node initialNode);

/**
 * @brief Create a Node safely (initializes the entire struct)
 * @param ip the ip string
 * @param port the 2-byte (16-bit) port
 * @param username the person's username string to use in chat messages
 * @param nodeOut the output Node created
 */
extern inline void createNode(char *ip, short port, char *username, Node *nodeOut);

extern inline void acceptNode(Node **nodeList, Node *nodeOut);
extern inline void addNode(Node **nodeList, Node *node);
extern inline void removeNode(Node **nodeList, Node *node);
