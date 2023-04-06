typedef struct {
    char *ip;
    unsigned short port;
    char *name;
} Node;

typedef struct {
    Node *node;
    Node *nextNode;
} NodeListItem;

typedef NodeListItem *NodeList;
