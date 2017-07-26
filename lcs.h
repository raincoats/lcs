#ifndef LCS_H
#define LCS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <locale.h>

// macro wrapper
#define mw(e) do { e; } while (0)

#define STDERR_MSG(colour, c, fmt, ...) \
		dprintf(2, "\e[38;5;" colour "m[" c "]\e[m " fmt, ## __VA_ARGS__);

// colours
#define DEBUG(fmt, ...) mw( STDERR_MSG("039", "?", fmt "\n", ## __VA_ARGS__) )
#define  WARN(fmt, ...) mw( STDERR_MSG("214", "!", fmt "\n", ## __VA_ARGS__) )
#define  PAIN(fmt, ...) mw( STDERR_MSG("124", "!", fmt "\n", ## __VA_ARGS__); _exit(1) )
#define  GOOD(fmt, ...) mw( STDERR_MSG("034", "$", fmt "\n", ## __VA_ARGS__) )
#define   DIE(fmt, ...) mw( STDERR_MSG("124", "!", fmt ": %m\n", ## __VA_ARGS__); _exit(1) )


// options (set in getopt)

enum output_formats {
	output_cidr,      // 22
	output_cidr_full, // 1.2.3.4/22
	output_mask,      // 255.255.252.0
	output_mask_full, // 1.2.0.0/255.255.252.0
	output_verbose
};

struct options {
	int       output_format;
	uint32_t  *addr[2];
};

// the main ip address type
typedef uint32_t ip_t;

// globals
static char *argv0; // just for --help and --version

#endif // LCS_H
