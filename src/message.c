#include "message.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

inline int getBit(uint8_t byteFlag, int bitNum) {
    return (byteFlag & (1 << (bitNum - 1)));
}

inline MessageType getMessageType(MessageHeader messageHeader) {
    // Get first three bits
    return (messageHeader >> 5);
}

inline void setMessageType(MessageHeader *messageHeader, MessageType messageType) {
    // Set first three bits
    *messageHeader = (*messageHeader & ~0xE0) | ((messageType << 5) & 0xE0);
}

inline Message createJoinMessage(char *name) {
    Message message;
    setMessageType(&message.header, MSG_JOIN);
    message.name = name;

    return message;
}

inline Message createAddMemberMessage(Node nodeInfo) {
    Message message;
    setMessageType(&message.header, MSG_ADD_MEMBER);
    message.nodeInfo = nodeInfo;

    return message;
}

inline Message createMemberListMessage(NodeListItem *memberList) {
    Message message;
    setMessageType(&message.header, MSG_MEMBER_LIST);
    message.memberList = memberList;

    return message;
}

inline Message createNoteMessage(char *note) {
    Message message;
    setMessageType(&message.header, MSG_NOTE);
    message.note = note;

    return message;
}

inline Message createLeaveMessage() {
    Message message;
    setMessageType(&message.header, MSG_LEAVE);

    return message;
}

inline Message createShutdownAllMessage() {
    Message message;
    setMessageType(&message.header, MSG_SHUTDOWN_ALL);

    return message;
}

inline extern inline size_t getNodeSize(Node *node) {
    size_t nodeLen = 0;

    nodeLen += strlen(node->ip) + 1;
    nodeLen += 2; // short (port)
    nodeLen += strlen(node->name);

    return nodeLen;
}

inline size_t getSerializedMessageSize(Message message) {
    // Start at one byte for the header
    size_t serializedMsgLen = 1;

    MessageType msgType = getMessageType(message.header);
    switch (msgType) {
        case MSG_JOIN:
            serializedMsgLen += strlen(message.name) + 1;
            break;
        case MSG_ADD_MEMBER:
            serializedMsgLen += getNodeSize(&message.nodeInfo);
            break;
        case MSG_MEMBER_LIST: {
            NodeListItem *curNodeItem = message.memberList;

            do {
                curNodeItem = curNodeItem->nextNode;
                serializedMsgLen += getNodeSize(curNodeItem);
            }
            while (message.memberList->nextNode != NULL);
            break;
        }
        case MSG_NOTE:
            serializedMsgLen += strlen(message.note) + 1;
            break;
    }

    return serializedMsgLen;
}

inline uint8_t *serializeNode(Node *node, size_t *rawNodeLenOut) {
    *rawNodeLenOut = getNodeSize(node);
    uint8_t *serializedNode = malloc(*rawNodeLenOut);

    *rawNodeLenOut += strlen(strcpy(serializedNode, node->ip)); // Copy IP
    memcpy(serializedNode + *rawNodeLenOut, &node->port, sizeof(node->port)); // Copy port
    *rawNodeLenOut += 2;
    *rawNodeLenOut += strlen(strcpy(serializedNode + *rawNodeLenOut, node->name)); // Copy username

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
            strcpy(rawMsg + rawMsgIndex, message.name);
            break;
        case MSG_ADD_MEMBER: {
            size_t rawNodeLen;
            memcpy(rawMsg + rawMsgIndex, serializeNode(&message.nodeInfo, &rawNodeLen), rawNodeLen);
            rawMsgIndex += rawNodeLen;
            break;
        }
        case MSG_MEMBER_LIST: {
            NodeListItem *curNodeItem = message.memberList;

            do {
                curNodeItem = curNodeItem->nextNode;
                size_t rawNodeLen;
                memcpy(rawMsg + rawMsgIndex, serializeNode(curNodeItem, &rawNodeLen), rawNodeLen);
                rawMsgIndex += rawNodeLen;
            }
            while (message.memberList->nextNode != NULL);
            break;
        }
        case MSG_NOTE:
            strcpy(rawMsg + rawMsgIndex, message.note);
            break;
    }

    return rawMsg;
}
