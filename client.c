#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "aliases.h"
#include "read_write_helpers.h"

Status sendMessage(int connectionFd, const char* message, Size messageLength)
{

    Size bufferLength = 4 + messageLength + 1;
    byte buffer[bufferLength];

    memcpy(buffer, &messageLength, sizeof(u32));
    memcpy(&buffer[sizeof(u32)], message, messageLength);

    buffer[bufferLength - 1] = '\0';

    writeAll(connectionFd, buffer, bufferLength);

    byte lengthBuffer[sizeof(u32)];
    char messageBuffer[MAX_MESSAGE_LENGTH + 1];
    u32 serverMessageLength;

    Status status = readFull(connectionFd, lengthBuffer, sizeof(u32));

    if (status == FAILED) {
        panic("Cannot read len from connection fd");
    }

    memcpy(&serverMessageLength, lengthBuffer, sizeof(u32));

    if (serverMessageLength > MAX_MESSAGE_LENGTH) {
        logError("Server message is too long");
        return FAILED;
    }

    status = readFull(connectionFd, messageBuffer, serverMessageLength);

    if (status == FAILED) {
        panic("Cannot read message from connection fd");
    }

    printf("Server says: %s\n", messageBuffer);
    close(connectionFd);

    return OK;
}

int main()
{
    int connectionFd = socket(AF_INET6, SOCK_STREAM, 0);
    if (connectionFd < 0) {
        panic("Cannot create socket()");
    }

    SocketAddressIPv6 addr = {
        .sin6_family = AF_INET6, // ipv6
        .sin6_port = (htons(1234)),
        // .sin6_addr.s6_addr = (ntohl(INADDR_LOOPBACK)) // 127.0.0.1
    };
    inet_pton(AF_INET6, "::1", &addr.sin6_addr);

    int rv = connect(connectionFd, (const struct sockaddr*)&addr, sizeof(addr));
    if (rv != 0) {
        panic("Cannot connect to server, aborting");
    }

    sendMessage(connectionFd, "hello", 5);

    return OK;
}