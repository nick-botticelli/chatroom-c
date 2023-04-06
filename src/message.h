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
