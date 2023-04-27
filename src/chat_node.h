#pragma once

#define PORT 0xCAFE // 51966



typedef struct {
    char *ip;
    unsigned short port;
    char *name;
} Node;

typedef struct NodeListItem {
    Node *node;
    struct NodeListItem *nextNode;
} NodeListItem;

typedef NodeListItem *NodeList;



Node *getNode(int index);
Node *addNode(Node *node);
Node *removeNode(Node *node);
Node *removeNodeAtIndex(int index);
