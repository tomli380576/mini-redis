#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <stdbool.h>

#include "aliases.h"
#include "read_write_helpers.h"

/**
 * reads exactly bufferLength number of bytes from connectionFd into outputBuffer
 * - Mutates outputBuffer
 */
Status readFull(int connectionFd, byte *outputBuffer, Size bufferLength)
{
    const Size fullBufferLength = bufferLength;

    while (bufferLength > 0)
    {
        Size offset = fullBufferLength - bufferLength;
        SignedSize numBytesRead = read(
            connectionFd,
            outputBuffer + offset,
            bufferLength);

        if (numBytesRead == -1 || (Size)numBytesRead > bufferLength)
        {
            // failed to read or read too much
            return FAILED;
        }

        bufferLength -= numBytesRead;
    }

    return OK;
}

/**
 * Writes exactly bufferLength bytes of data from inputBuffer into connectionFd
 * - Mutates connectionFd
 */
Status writeAll(int connectionFd, byte *inputBuffer, Size bufferLength)
{

    const Size fullBufferLength = bufferLength;

    while (bufferLength > 0)
    {
        Size offset = fullBufferLength - bufferLength;
        SignedSize numBytesWritten = write(
            connectionFd,
            inputBuffer + offset,
            bufferLength);

        if (numBytesWritten == -1 || (Size)numBytesWritten > bufferLength)
        {
            return FAILED;
        }

        bufferLength -= numBytesWritten;
    }

    return OK;
}

void info(const char* message)
{
    fprintf(stdout, "[INFO] %s\n", message);
}

void logError(const char* message)
{
    fprintf(stderr, "[ERROR] %s\n", message);
}

void panic(const char* message)
{
    fprintf(stderr, "[FATAL] %s\n", message);
    abort();
}
