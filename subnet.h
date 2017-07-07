#ifndef SUBNET_H
#define SUBNET_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <arpa/inet.h>

// macro wrapper
#define mw(e) do { e; } while (0)

#define STDERR_MSG(colour, c, fmt, ...) \
		dprintf(2, "\e[38;5;" colour "m[" c "]\e[m " fmt, ## __VA_ARGS__);

// colours
#define DEBUG(fmt, ...) mw( STDERR_MSG("039", "?", fmt "\n", ## __VA_ARGS__) )
#define  WARN(fmt, ...) mw( STDERR_MSG("214", "!", fmt ": %m\n", ## __VA_ARGS__) )
#define  PAIN(fmt, ...) mw( STDERR_MSG("124", "!", fmt "\n", ## __VA_ARGS__); _exit(1) )
#define  GOOD(fmt, ...) mw( STDERR_MSG("034", "$", fmt "\n", ## __VA_ARGS__) )
#define   DIE(fmt, ...) mw( STDERR_MSG("124", "!", fmt ": %m\n", ## __VA_ARGS__); _exit(1) )
#endif // SUBNET_H
