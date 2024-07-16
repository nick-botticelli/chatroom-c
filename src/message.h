#pragma once

#include <stdint.h>
#include <stdio.h>

#include "chat_node.h"

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
 * 
 * For use with getBit and setBit, the order is flipped!
 */
typedef uint8_t MessageHeader;

typedef enum {
    MSG_JOIN,
    MSG_WELCOME,
    MSG_ADD_MEMBER,
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
            short port;
            // Flag 5 = boolean, whether client is joining new chat room (1) or adding existing members (0)
        };

        // Welcome
        struct {
            char *remoteUsername;
        };

        // Add member
        struct {
            Node *nodeInfo;
        };

        // Note
        struct {
            char *note;
        };

        // Leave
        struct {
            // Flag 5 = boolean, whether to shutdown chat room (1) or not (0)
        };
    };
} Message;



/**
 * @brief Get the bit value located at the bit number
 * @param byteFlag 1-byte value to retrieve bit value from
 * @param bitNum The bit number to retrieve starting from the right; 0-7
 * @return bit value; true or false for 1 or 0 respectively
 */
extern inline bool getBit(uint8_t byteFlag, int bitNum);

extern inline void setBit(uint8_t *byteFlag, int bitNum, bool bitValue);

extern inline MessageType getMessageType(MessageHeader messageHeader);
extern inline void setMessageType(MessageHeader *messageHeader, MessageType messageType);

extern Message createJoinMessage(char *username, short port, bool newJoin);
extern Message createWelcomeMessage(char *username);
extern Message createAddMemberMessage(Node *nodeInfo);
extern Message createNoteMessage(char *note);
extern Message createLeaveMessage(bool shutdownAll);

extern size_t getNodeSize(Node *node);
extern size_t getSerializedMessageSize(Message message);
extern uint8_t *serializeNode(Node *node, size_t *rawNodeLenOut);
extern uint8_t *serializeMessage(Message message, size_t *serializedMessageLenOut);

extern Node *deserializeNode(Node *nodeList, uint8_t *rawNode);
extern Message deserializeMessage(Node *nodeList, uint8_t *rawMessage, size_t rawMessageSize);
