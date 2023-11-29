#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "aliases.h"
#include "connection_state.h"
#include "read_write_helpers.h"

// fd is file descriptor

static Status handleOneRequest(int connectionFd)
{
    /**
        +-----+------+-----+------+--------
        | len | msg1 | len | msg2 | more...
        +-----+------+-----+------+--------
    */

    // read the length, the message, and 1 extra byte for \0 character
    const Size readBufferLength = sizeof(u32) + MAX_MESSAGE_LENGTH + 1;
    byte readBuffer[readBufferLength];

    Status status = readFull(connectionFd, readBuffer, 4);

    if (status == FAILED) {
        return FAILED;
    }

    u32 clientMessageLength = 0;
    memcpy(&clientMessageLength, readBuffer, 4);

    if (clientMessageLength > MAX_MESSAGE_LENGTH) {
        logError("Message is too long");
        return FAILED;
    }

    // skip first 4 bytes, then read into the buffer
    Status readStatus = readFull(connectionFd, &readBuffer[4], clientMessageLength);

    if (readStatus == FAILED) {
        logError("readFull failed");
        return FAILED;
    }

    // done reading, now terminate the buffer
    readBuffer[readBufferLength - 1] = '\0';
    printf("Client says: %s\n", &readBuffer[4]);

    // now reply to the client
    const char replyBase[] = "got your message, you said: ";
    const u32 replyLength = strlen(replyBase) + clientMessageLength;
    const Size writeBufferSize = 4 + replyLength + 1;
    byte writeBuffer[writeBufferSize];

    // copy the message and length into the write buffer
    memcpy(writeBuffer, &replyLength, sizeof(u32));
    memcpy(&writeBuffer[sizeof(u32)], replyBase, strlen(replyBase));
    memcpy(&writeBuffer[sizeof(u32) + strlen(replyBase) ], &readBuffer[4], clientMessageLength);

    writeBuffer[writeBufferSize - 1] = '\0';

    return writeAll(connectionFd, writeBuffer, writeBufferSize);
}

static Connection initializeConnection(int connectionFd)
{
    Connection connection = {
        .connectionFd = connectionFd,
        .readBufferSize = 0,
        .writeBufferSentSize = 0,
        .writeBufferSize = 0,
        .state = STATE_REQUEST
    };

    return connection;
}

static void sayHi(int connectionFd)
{
    char readBuffer[64];
    SignedSize numBytesRead = read(connectionFd, readBuffer, sizeof(readBuffer) - 1);

    if (numBytesRead == -1) {
        logError("read() error");
        return;
    }

    readBuffer[numBytesRead] = '\0';
    printf("client says: %s\n", readBuffer);

    char writeBuffer[] = "henno";
    write(connectionFd, writeBuffer, strlen(writeBuffer));
}

static Status enableNonBlocking(int fileDescriptor)
{
    errno = 0;

    // get file descriptor flags
    const int flags = fcntl(fileDescriptor, F_GETFD);
    if (errno) {
        logError("fcntl() get error");
        return FAILED;
    }

    // set non blocking to true by or-ing with the bitmask
    const int newFlags = flags | O_NONBLOCK;
    // write the new fire descriptor flag
    fcntl(fileDescriptor, F_SETFD, newFlags);

    if (errno) {
        logError("fcntl() set error");
        return FAILED;
    }

    return OK;
}

int main()
{
    const u32 port = 1234;
    const u32 address = 0;
    const u32 socketFd = socket(AF_INET6, SOCK_STREAM, 0);

    if (socketFd < 0) {
        panic("failed at socket()");
    }

    u32 optionValue; // unused, but needed for setsockopt
    setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));

    SocketAddressIPv6 addr = {
        .sin6_family = AF_INET6, // IPV4
        .sin6_port = (ntohs(port)), // converts from network byte order to host byte order
        .sin6_addr.s6_addr = (ntohl(address)) // same thing, but for unsigned long
    };

    if (bind(socketFd, (const struct sockaddr*)&addr, sizeof(addr)) != 0) {
        printf("%d", errno);
        panic("failed at bind");
    }

    if (listen(socketFd, SOMAXCONN) != 0) {
        panic("failed at listen");
    }

    printf("[INFO] Server is running at %d! (Try running a client)\n", port);

    while (true) {
        SocketAddressIPv6 clientAddress;
        socklen_t socketLength = sizeof(clientAddress);

        info("Waiting for connection");

        int connectionFd = accept(socketFd, (struct sockaddr*)&clientAddress, &socketLength);
        // ^^ accept() blocks the loop until a new connection comes in
        printf("Got new connection at descriptor %d!\n", connectionFd);

        if (connectionFd < 0) {
            logError("accept() failed");
            continue;
        }

        handleOneRequest(connectionFd);
        // sayHi(connectionFd);
        close(connectionFd);
    }

    return 0;
}