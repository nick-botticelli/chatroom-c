#include "debug.h"

#include <stdlib.h>


void debug_hexdump(void *data, size_t size) {
#ifndef NDEBUG
	char ascii[17];
	ascii[16] = '\0';

	for (size_t i = 0; i < size; i++) {
		printf("%02X ", ((unsigned char *) data)[i]);

		if (((unsigned char *) data)[i] >= ' ' && ((unsigned char *) data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char *) data)[i];
		} else {
			ascii[i % 16] = '.';
		}

		if ((i + 1) % 8 == 0 || i + 1 == size) {
			printf(" ");

			if ((i + 1) % 16 == 0) {
				printf("|  %s \n", ascii);
			}
            else if (i + 1 == size) {
				ascii[(i + 1) % 16] = '\0';

				if ((i + 1) % 16 <= 8) {
					printf(" ");
				}

				for (size_t j = (i + 1) % 16; j < 16; j++) {
					printf("   ");
				}

				printf("|  %s \n", ascii);
			}
		}
	}
#endif
}

void printNodeList(Node *nodeList) {
#ifndef NDEBUG
    Node *curNode = nodeList;

    // Traverse linked list until we find the node to remove
    printf("===== Node list =====\n");
    while (curNode != NULL) {
        printf("%p: %s:%d - %s -\t-> %p\n", curNode, curNode->ip, curNode->port, curNode->username, curNode->nextNode);
        curNode = curNode->nextNode;
    }
    printf("=====================\n");
#endif
}

void debugMessage(Message message) {
#ifndef NDEBUG
    MessageType messageType = getMessageType(message.header);

    printf("Message {\n");
    printf("\theader:\n");
    printf("\t\ttype = %d\n", messageType);

    switch (messageType) {
        case MSG_JOIN:
            printf("\tusername = %s\n", message.username);
            printf("\tport = %d\n", message.port);
            break;
        case MSG_WELCOME:
            printf("\tremoteUsername = %s\n", message.remoteUsername);
        case MSG_ADD_MEMBER:
            printf("\tWIP\n");
            break;
        case MSG_NOTE:
            printf("\tnote = %s\n", message.note);
            break;
        case MSG_LEAVE:
            printf("\tshutdownAll = %s\n", getBit(message.header, 0) ? "true" : "false");
            break;
    }

    printf("}\n");
#endif
}
