#ifndef READ_WRITE_HELPERS_H
#define READ_WRITE_HELPERS_H

#include "aliases.h"

Status readFull(int connectionFd, byte* outputBuffer, Size bufferLength);
Status writeAll(int connectionFd, byte* inputBuffer, Size bufferLength);

void info(const char* message);
void logError(const char* message);
void panic(const char* message);

#endif