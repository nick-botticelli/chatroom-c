#include "message.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"

inline bool getBit(uint8_t byteFlag, int bitNum) {
    return byteFlag & (1 << (bitNum - 1));
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

inline Message createJoinMessage(char *username) {
    Message message;
    message.header = BLANK_HEADER;
    setMessageType(&message.header, MSG_JOIN);
    message.username = username;

    return message;
}

inline Message createAddMemberMessage(Node *nodeInfo) {
    Message message;
    message.header = BLANK_HEADER;
    setMessageType(&message.header, MSG_ADD_MEMBER);
    message.nodeInfo = nodeInfo;

    return message;
}

inline Message createMemberListMessage(Node *nodeList) {
    Message message;
    message.header = BLANK_HEADER;
    setMessageType(&message.header, MSG_MEMBER_LIST);
    message.nodeList = nodeList;

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
    size_t nodeLen = 0;

    nodeLen += strlen(node->ip) + 1;
    nodeLen += 2; // short (port)
    nodeLen += strlen(node->username);

    return nodeLen;
}

inline size_t getSerializedMessageSize(Message message) {
    // Start at one byte for the header
    size_t serializedMsgLen = 1;

    MessageType msgType = getMessageType(message.header);
    switch (msgType) {
        case MSG_JOIN:
            serializedMsgLen += strlen(message.username) + 1;
            break;
        case MSG_ADD_MEMBER:
            serializedMsgLen += getNodeSize(message.nodeInfo);
            break;
        case MSG_MEMBER_LIST: {
            Node *curNodeItem = message.nodeList;

            do {
                curNodeItem = curNodeItem->nextNode;
                serializedMsgLen += getNodeSize(curNodeItem);
            }
            while (message.nodeList->nextNode != NULL);
            break;
        }
        case MSG_NOTE:
            serializedMsgLen += strlen(message.note) + 1;
            break;
        default:
            break;
    }

    return serializedMsgLen;
}

inline uint8_t *serializeNode(Node *node, size_t *rawNodeLenOut) {
    *rawNodeLenOut = getNodeSize(node);
    uint8_t *serializedNode = malloc(*rawNodeLenOut);

    *rawNodeLenOut += strlen(strcpy((char *) serializedNode, node->ip)); // Copy IP
    memcpy(serializedNode + *rawNodeLenOut, &node->port, sizeof(node->port)); // Copy port
    *rawNodeLenOut += 2;
    *rawNodeLenOut += strlen(strcpy((char *) serializedNode + *rawNodeLenOut, node->username)); // Copy username

    return serializedNode;
}

inline uint8_t *serializeMessage(Message message, size_t *rawMsgLenOut) {
    uint8_t *rawMsg;
    size_t rawMsgIndex;
    *rawMsgLenOut = getSerializedMessageSize(message);

    rawMsg = malloc(*rawMsgLenOut);

    rawMsg[0] = message.header;
    rawMsgIndex = 1;

    MessageType msgType = getMessageType(message.header);
    switch (msgType) {
        case MSG_JOIN:
            strcpy((char *) rawMsg + rawMsgIndex, message.username);
            break;
        case MSG_ADD_MEMBER: {
            size_t rawNodeLen;
            uint8_t *serializedNode = serializeNode(message.nodeInfo, &rawNodeLen);
            memcpy(rawMsg + rawMsgIndex, serializedNode, rawNodeLen);
            rawMsgIndex += rawNodeLen;
            free(serializedNode);
            break;
        }
        case MSG_MEMBER_LIST: {
            Node *curNode = message.nodeList;

            do {
                curNode = curNode->nextNode;
                size_t rawNodeLen;
                uint8_t *serializedNode = serializeNode(curNode, &rawNodeLen);
                memcpy(rawMsg + rawMsgIndex, serializedNode, rawNodeLen);
                rawMsgIndex += rawNodeLen;
                free(serializedNode);
            }
            while (message.nodeList->nextNode != NULL);
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

inline void deserializeMessage(uint8_t *messageRaw, Message *messageOut) {
    debug("deserializeMessage()");
}
