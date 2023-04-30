#pragma once

#include <stdint.h>
#include <stdio.h>

#include <chat_node.h>

#define BLANK_HEADER (MessageHeader) 0

// One byte for header
#define MAX_PAYLOAD_SIZE 127

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
    MSG_LEAVE
} MessageType;

typedef struct {
    MessageHeader header;

    // Data payload
    union {
        // Join
        struct {
            char *username;
        };

        // Add member
        struct {
            Node *nodeInfo;
        };

        // Member list
        struct {
            Node *nodeList;
        };

        // Note
        struct {
            char *note;
        };

        // Leave
        struct {
            // Flag 5 = boolean, whether to shutdown chat room (1) or not (0)
        };

        // Shutdown all
        // struct {
        // };
    };
} Message;



extern inline bool getBit(uint8_t byteFlag, int bitNum);
extern inline void setBit(uint8_t *byteFlag, int bitNum, bool bitValue);

extern inline MessageType getMessageType(MessageHeader messageHeader);
extern inline void setMessageType(MessageHeader *messageHeader, MessageType messageType);

extern inline Message createJoinMessage(char *username);
extern inline Message createAddMemberMessage(Node *nodeInfo);
extern inline Message createMemberListMessage(Node *nodeList);
extern inline Message createNoteMessage(char *note);
extern inline Message createLeaveMessage(bool shutdownAll);

extern inline size_t getNodeSize(Node *node);
extern inline size_t getSerializedMessageSize(Message message);
extern inline uint8_t *serializeNode(Node *node, size_t *rawNodeLenOut);
extern inline uint8_t *serializeMessage(Message message, size_t *serializedMessageLenOut);

extern inline void deserializeMessage(uint8_t *messageRaw, Message *messageOut);
