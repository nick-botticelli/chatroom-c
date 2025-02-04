#include "receiver_handler.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "debug.h"
#include "chat_node.h"
#include "sender_handler.h"

bool handleClient(Node *nodeList, Node *node, Message message) {
    MessageType messageType = getMessageType(message.header);

    switch (messageType) {
        case MSG_JOIN:
            debug("Join received!");

            // Send welcome message with username
            Message welcomeMessage = createWelcomeMessage(nodeList->username);
            sendMessage(node->sock, welcomeMessage);

            // Send add member message to all other members of the chat room
            node->username = message.username;
            node->port = message.port;
            debug("Joined client (%p) port: %d", node, node->port);

            printf("%s has joined the chat room.\n", node->username);

            if (getBit(message.header, 0)) {
                // Only send add member if the member is newly joining
                Message addMemberMessage = createAddMemberMessage(node);
                sendMessageAll(nodeList, node, addMemberMessage);
            }
            printNodeList(nodeList);

            break;
        case MSG_WELCOME:
            debug("Welcome received!");

            node->username = message.remoteUsername;
            printNodeList(nodeList);

            printf("%s has joined the chat room.\n", node->username);

            break;
        case MSG_ADD_MEMBER:
            debug("Add member received!");

            Message joinMessage;
            if (connectToNode(nodeList, message.nodeInfo, false, &joinMessage)) {
                sendMessage(message.nodeInfo->sock, joinMessage);
            }

            break;
        case MSG_NOTE:
            debug("Note received!");
            printf("%s> %s\n", node->username, message.note);
            break;
        case MSG_LEAVE:
            debug("Leave received!");
            
            if (getBit(message.header, 0)) {
                printf("The chat room is shutting down. Goodbye.\n");
                return false;
            }

            // Disconnect from node (mark for removal); this prevents any more messages from being received
            node->connected = false;

            printf("%s has left the chat room.\n", node->username);

            break;
        default:
            debug("Error: Invalid message type received (%d)!", messageType);
    }

    return true;
}

bool receiveMessage(Node *nodeList, Node *node, Message *messageOut) {
    uint8_t rawMessage[MAX_PAYLOAD_SIZE + 1];
    ssize_t rawMessageSize = recv(node->sock, &rawMessage, sizeof(rawMessage), 0);

    if (rawMessageSize == -1) {
        debug("rawMessageSize == -1");
        if (errno == EBADF) {
            debug("Error: errno == EBADF");
            goto SOCKET_CLOSED;
        }

        debug("Error reading message?");
        return false;
    }
    else if (rawMessageSize == 0) {
        debug("rawMessageSize == 0");
        goto SOCKET_CLOSED;
    }

    debug("Received message!");

    // Create message from received data
    *messageOut = deserializeMessage(nodeList, rawMessage, rawMessageSize);
    return true;

SOCKET_CLOSED:
    debug("Socket closed for node %p (%s)!", node, node->username);
    node->connected = false;
    return false;
}
