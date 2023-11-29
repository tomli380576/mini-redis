#ifndef ALIASES_H
#define ALIASES_H

#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>

#define MAX_MESSAGE_LENGTH 4096

// Basic Types
typedef int32_t i32;
typedef u_int32_t u32;
typedef u_int8_t u8;
typedef float f32;
typedef double f64;
typedef size_t Size;
typedef char byte;
/** A signed size can be -1 */
typedef ssize_t SignedSize;
typedef struct sockaddr_in SocketAddressIPv4;
typedef struct sockaddr_in6 SocketAddressIPv6;

typedef enum { FAILED = -1, OK = 0 } Status;

#endif