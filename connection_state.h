#ifndef CONNECTION_STATES_H
#define CONNECTION_STATES_H

#include "aliases.h"
#include <stddef.h>
#include <stdint.h>

typedef enum {
    STATE_REQUEST = 0,
    STATE_RESPOND = 1,
    STATE_END = 2,
} ConnectionState;

typedef struct {
    int connectionFd;
    ConnectionState state; // requested or responded
    // buffer for reading
    Size readBufferSize;
    byte readBuffer[sizeof(u32) + MAX_MESSAGE_LENGTH + 1];
    // buffer for writing
    Size writeBufferSize;
    Size writeBufferSentSize;
    byte writeBuffer[sizeof(u32) + MAX_MESSAGE_LENGTH + 1];
} Connection;

#endif