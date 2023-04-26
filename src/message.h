#pragma once

#include <stdint.h>

#include <chat_node.h>

typedef uint8_t PacketHeader;

typedef struct {
    PacketHeader header;

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
            char *message;
        };

        // Leave
        struct {
        };

        // Shutdown all
        struct {
        };
    };
} Message;



Message *createJoinMessage(char *name);
Message *createAddMemberMessage(Node *nodeInfo);
Message *createMemberListMessage(NodeListItem *memberList);
Message *createNoteMessage(char *message);
Message *createLeaveMessage();
Message *createShutdownAllMessage();
