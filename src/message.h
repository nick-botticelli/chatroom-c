#pragma once

#include <stdint.h>
#include <stdio.h>

#include <chat_node.h>

#define BLANK_HEADER (MessageHeader) 0

/**
 * Bits:
 * 0: Packet type
 * 1: Packet type
 * 2: Packet type
 * 3: Flag 1
 * 4: Flag 2
 * 5: Flag 3
 * 6: Flag 4
 * 7: Flag 5
 */
typedef uint8_t MessageHeader;

typedef enum {
    MSG_JOIN,
    MSG_ADD_MEMBER,
    MSG_MEMBER_LIST,
    MSG_NOTE,
    MSG_LEAVE,
    MSG_SHUTDOWN_ALL
} MessageType;

typedef struct {
    MessageHeader header;

    // Data payload
    union {
        // Join
        struct {
            char *name;
        };

        // Add member
        struct {
            Node nodeInfo;
        };

        // Member list
        struct {
            NodeListItem *memberList;
        };

        // Note
        struct {
            char *note;
        };

        // Leave
        struct {
        };

        // Shutdown all
        struct {
        };
    };
} Message;



extern inline int getBit(uint8_t byteFlag, int bitNum);

extern inline MessageType getMessageType(MessageHeader messageHeader);
extern inline void setMessageType(MessageHeader *messageHeader, MessageType messageType);

extern inline Message createJoinMessage(char *name);
extern inline Message createAddMemberMessage(Node nodeInfo);
extern inline Message createMemberListMessage(NodeListItem *memberList);
extern inline Message createNoteMessage(char *note);
extern inline Message createLeaveMessage();
extern inline Message createShutdownAllMessage();

extern inline size_t getNodeSize(Node *node);
extern inline size_t getSerializedMessageSize(Message message);
extern inline uint8_t *serializeNode(Node *node, size_t *rawNodeLenOut);
extern inline uint8_t *serializeMessage(Message message, size_t *serializedMessageLenOut);
extern inline Message *deserializeMessage(uint8_t *rawMessage);
