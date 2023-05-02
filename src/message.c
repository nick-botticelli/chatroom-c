#include "message.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "main.h"
#include "sender_handler.h"

inline bool getBit(uint8_t byteFlag, int bitNum) {
    return byteFlag & (1 << bitNum);
}

inline void setBit(uint8_t *byteFlag, int bitNum, bool bitValue) {
    *byteFlag |= bitValue << bitNum;
}

inline MessageType getMessageType(MessageHeader messageHeader) {
    // Get first three bits
    return (messageHeader >> 5);
}

inline void setMessageType(MessageHeader *messageHeader, MessageType messageType) {
    // Set first three bits
    *messageHeader = (*messageHeader & ~0xE0) | ((messageType << 5) & 0xE0);
}

inline Message createJoinMessage(char *username, short port, bool newJoin) {
    debug("createJoinMessage() port: %d", port);
    Message message;
    message.header = BLANK_HEADER;
    setMessageType(&message.header, MSG_JOIN);
    setBit(&message.header, 0, newJoin);

    message.username = username;
    message.port = port;

    return message;
}

inline Message createWelcomeMessage(char *remoteUsername) {
    Message message;
    message.header = BLANK_HEADER;
    setMessageType(&message.header, MSG_WELCOME);
    message.remoteUsername = remoteUsername;

    return message;
}

inline Message createAddMemberMessage(Node *nodeInfo) {
    Message message;
    message.header = BLANK_HEADER;
    setMessageType(&message.header, MSG_ADD_MEMBER);
    message.nodeInfo = nodeInfo;

    return message;
}

inline Message createNoteMessage(char *note) {
    Message message;
    message.header = BLANK_HEADER;
    setMessageType(&message.header, MSG_NOTE);
    message.note = note;

    return message;
}

inline Message createLeaveMessage(bool shutdownAll) {
    Message message;
    message.header = BLANK_HEADER;
    setMessageType(&message.header, MSG_LEAVE);

    // Shutdown all flag (Flag 5)
    setBit(&message.header, 0, shutdownAll);

    return message;
}

inline size_t getNodeSize(Node *node) {
    size_t nodeSize = 0;

    nodeSize += strlen(node->ip) + 1;
    nodeSize += sizeof(short); // TODO: short (port)?
    nodeSize += strlen(node->username) + 1;

    return nodeSize;
}

inline size_t getSerializedMessageSize(Message message) {
    // Start at one byte for the header
    size_t serializedMsgLen = 1;

    MessageType messageType = getMessageType(message.header);
    switch (messageType) {
        case MSG_JOIN:
            serializedMsgLen += strlen(message.username) + 1;
            serializedMsgLen += sizeof(message.port);
            break;
        case MSG_WELCOME:
            serializedMsgLen += strlen(message.remoteUsername) + 1;
            break;
        case MSG_ADD_MEMBER:
            serializedMsgLen += getNodeSize(message.nodeInfo);
            break;
        case MSG_NOTE:
            serializedMsgLen += strlen(message.note) + 1;
            break;
        default:
            break;
    }

    debug("getSerializedMessageSize: %zu", serializedMsgLen);

    return serializedMsgLen;
}

inline uint8_t *serializeNode(Node *node, size_t *rawNodeLenOut) {
    *rawNodeLenOut = getNodeSize(node);
    uint8_t *serializedNode = malloc(*rawNodeLenOut);
    size_t curIndex = 0;

    curIndex += strlen(strcpy((char *) serializedNode, node->ip)) + 1; // Copy IP
    memcpy(serializedNode + curIndex, &node->port, sizeof(node->port)); // Copy port; TODO: Simply
    curIndex += sizeof(short); // TODO: Use short type?
    curIndex += strlen(strcpy((char *) serializedNode + curIndex, node->username)) + 1; // Copy username

    return serializedNode;
}

inline uint8_t *serializeMessage(Message message, size_t *rawMsgLenOut) {
    uint8_t *rawMsg;
    size_t rawMsgIndex;
    *rawMsgLenOut = getSerializedMessageSize(message);

    rawMsg = malloc(*rawMsgLenOut);

    rawMsg[0] = message.header;
    rawMsgIndex = 1;

    MessageType messageType = getMessageType(message.header);
    switch (messageType) {
        case MSG_JOIN:
            strcpy((char *) rawMsg + rawMsgIndex, message.username);
            debug("serializeMessage() message.port: %d", message.port);
            debug("serializeMessage() sizeof(message.port): %lu\t|\tsizeof(short): %lu", sizeof(message.port), sizeof(short));
            memcpy((char *) rawMsg + rawMsgIndex + strlen(message.username) + 1, &message.port, sizeof(message.port));
            break;
        case MSG_WELCOME:
            strcpy((char *) rawMsg + rawMsgIndex, message.remoteUsername);
            break;
        case MSG_ADD_MEMBER: {
            size_t rawNodeLen;
            uint8_t *serializedNode = serializeNode(message.nodeInfo, &rawNodeLen);
            debug("serializedNode hexdump:");
            debug_hexdump(serializedNode, rawNodeLen);
            memcpy(rawMsg + rawMsgIndex, serializedNode, rawNodeLen);
            rawMsgIndex += rawNodeLen;
            free(serializedNode);
            break;
        }
        case MSG_NOTE:
            strcpy((char *) rawMsg + rawMsgIndex, message.note);
            break;
        default:
            break;
    }

    return rawMsg;
}

inline Node *deserializeNode(Node *nodeList, uint8_t *rawNode) {
    size_t rawNodeIndex = 0;

    // Parse IP
    size_t rawNodeIpSize = strlen((char *) rawNode) + 1;
    char *ip = malloc(rawNodeIpSize);
    strcpy(ip, (char *) rawNode);
    rawNodeIndex += rawNodeIpSize;

    // Parse port
    short port;
    memcpy(&port, rawNode + rawNodeIndex, sizeof(port));
    rawNodeIndex += sizeof(short);

    // Parse username
    size_t rawNodeUsernameSize = strlen((char *) rawNode + rawNodeIndex) + 1;
    char *username = malloc(rawNodeUsernameSize);
    strcpy(username, (char *) rawNode + rawNodeIndex);

    Node *node = createNode(ip, port, username, true, false);
    // connectSocket(nodeList, node); // TODO: ?

    return node;
}

inline Message deserializeMessage(Node *nodeList, uint8_t *rawMessage, size_t rawMessageSize) {
    Message message;

    debug("Received message hexdump:");
    debug_hexdump(rawMessage, rawMessageSize);

    message.header = rawMessage[0];

    MessageType messageType = getMessageType(message.header);
    switch (messageType) {
        case MSG_JOIN: {
            size_t usernameSize = strlen((char *) rawMessage + 1) + 1;
            message.username = malloc(usernameSize);
            strcpy(message.username, (char *) rawMessage + 1);
            memcpy(&message.port, rawMessage + 1 + usernameSize, sizeof(message.port)); // TODO: Network order
            debug("message.port = %d", message.port);
            break;
        }
        case MSG_WELCOME: {
            size_t remoteUsernameSize = strlen((char *) rawMessage + 1) + 1;
            message.username = malloc(remoteUsernameSize);
            strcpy(message.remoteUsername, (char *) rawMessage + 1);
            debug("message.remoteUsername = %s", message.remoteUsername);
            break;
        }
        case MSG_ADD_MEMBER: {
            Node *serializedNode = deserializeNode(nodeList, rawMessage + 1);
            message.nodeInfo = serializedNode;
            break;
        }
        case MSG_NOTE:
            message.note = malloc(strlen((char *) rawMessage + 1)) + 1;
            strcpy(message.note, (char *) rawMessage + 1);
            break;
        default:
            debug("Unknown message type recieved!");
            break;
    }

    return message;
}
