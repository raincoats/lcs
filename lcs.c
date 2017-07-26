/*
 *   lcs - lowest common subnet
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   find the smallest subnet that contains 2 given ipv4 addresses
 * 
 */
#include "lcs.h"

static char *PROG_NAME = "lcs";
static char *PROG_VERS = "1.0";


/* 
 * ntop - ip to string
 * made these ntop/pton functions because endianness on the inet_ ones
 * was ruxing me
 */
char *
ntop(ip_t ip)
{
	char *ret;
	uint8_t octet[4];
	int i;

	for (i=0; i<4; i++)
	{
		octet[i] = ip;
		ip >>= 8;
	}

	asprintf(&ret, "%u.%u.%u.%u", octet[3], octet[2], octet[1], octet[0]);
	return ret;
}

/*
 *  pton - string to ip
 */
ip_t
pton(char *orig_str)
{
	ip_t ip = 0;
	char *str = strdup(orig_str);
	int i;

	if (str == NULL)
		DIE("(internal error) %s str is NULL, orig_str='%s'",
			__func__, orig_str);

	/*
	 *   this loop ends up looking like 
	 *       ip = 0.0.0.1
	 *       ip = 0.0.1.2
	 *       ip = 0.1.2.3
	 *       ip = 1.2.3.4
	*/
	for (i=0; i<4; i++)
	{
		char *s = strsep(&str, ".");
		if (s == NULL)
			PAIN("(internal error) strsep returned null for i=%d! "
				 "str='%s'", i, str);

		int tmp = atoi(s);
		if (tmp > 255 || tmp < 0)
			PAIN("%s: octet '%d' is not in range 0..255", orig_str, tmp);

		/* shifting octets upwards here. (0.0.0.1 << 8 = 0.0.1.0) */
		ip <<= 8;
		ip += (uint8_t)tmp;
	}

	free(str);
	return ip;
}


/*
 *   looks_like_ip - does it look like an ipv4 address? checks it has 4 tokens
 *   seperated by dots
 */
int
looks_like_ip(char *orig_ip)
{
	int count = 0;
	char *ip = strdup(orig_ip);

	while (strsep(&ip, ".") != NULL)
		count++;

	free(ip);
	return count == 4;
}


/*
 *   cidr_from_mask - count how many bits are set in the address
 */
int
cidr_from_mask(ip_t mask)
{
	int cidr = 0;
	int i = sizeof(ip_t) * 8;

	/*
	 *   if bit at position i is set, cidr++
	 *   so for 0b111100000000 cidr would be 4
	 */
	while ((mask >> --i) & 1)
		cidr++;

	return cidr;
}

/*
 *   ip_sort - put the lower ip before the higher one
 */
void
ip_sort(ip_t *addr)
{
	ip_t tmp;
	
	if (addr[0] <= addr[1])
		return;

	tmp = addr[0];
	addr[0] = addr[1];
	addr[1] = tmp;
}

/*
 *   usage - help text
 */
void
usage(void)
{
	printf(
		"Usage: %s [OPTION]... <address1> <address2>\n"
		"Find the lowest common subnet of two ip addresses.\n"
		"\n"
		"  -c    print subnet in cidr format   (ex: 10.0.0.0/24)   (default)\n"
		"  -m    print subnet with long mask   (ex: 10.0.0.0/255.255.255.0)\n"
		"  -C    print just the cidr number    (ex: 24)\n"
		"  -M    print just the subnet mask    (ex: 255.255.255.0)\n"
		"  -v    print everything above\n"
		"  -h    display this help and exit\n"
		"  -V    output version information and exit\n",
		PROG_NAME
	);
	exit(0);
}

/*
 *   version - version info
 */
void
version(void)
{
	printf(
		"%s version %s\n"
		"<https://github.com/raincoats/lcs>\n",
		PROG_NAME, PROG_VERS);
	exit(0);
}

/*
 *   do_getopt - parse options, get ip's for the rest of the program
 */
void
do_getopt(struct options *o, int argc, char **argv)
{
	int c, i, ip_count=0;

	while ((c = getopt(argc, argv, "hVCMcmv")) != -1)
	{
		switch(c)
		{
			case 'h': usage();   break;
			case 'V': version(); break;
			case 'C': o->output_format = output_cidr;      break;
			case 'M': o->output_format = output_mask;      break;
			case 'c': o->output_format = output_cidr_full; break;
			case 'm': o->output_format = output_mask_full; break;
			case 'v': o->output_format = output_verbose;   break;
			case '?':
				WARN("unknown argument: -%c", optopt);
			default:
				break;
		}
	}

	/* get all the ip's passed to the prog. ip_count is how many done so far. */
	for (i=optind; i<argc; i++)
	{
		/* ignore any extras or non-ip args */
		if (ip_count == 2)
			break;
		if (! looks_like_ip(argv[i]))
			continue;

#ifdef __APPLE__
		/*
		 *  OS X's getopt is really really weird. i don't know if it's something
		 *  i'm doing wrong, but getopt seems to just give up when it first
		 *  encounters an non-option argument.
		 *  
		 *  the following argv's will not work with this prog on OS X, but will
		 *  work fine on linux:
		 *      ./lcs  1.2.3.4  -m  3.4.5.6
		 *      ./lcs  1.2.3.4  3.4.5.6  -m
		 *      
		 *  however this argv works fine:
		 *      ./lcs  -m  1.2.3.4  3.4.5.6
		 *
		 *  so as a quick hack just warn on any optargs that aren't directly 
		 *  adjacent to argv[0].
		 *
		 * FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME
		 */
		if (*(argv[i]) == '-')
		{
			WARN("option ignored: -%c (please put it before your non-option "
				 "args) (known bug)", *(argv[i]+1));
			continue;
		}
#endif

		*(o->addr[ip_count]) = pton(argv[i]);
		ip_count++;
	}

	/* if we were only passed one ip */
	if (ip_count != 2)
		usage();
}


void
do_output(struct options o, ip_t *addr, ip_t mask)
{
	/* string representations of the ip's */
	char *network = ntop(addr[0] & mask); /* network minimum address */
	char *netmask = ntop(mask);
	int cidr = cidr_from_mask(mask);

	switch (o.output_format)
	{
		case output_cidr:      printf("%d", cidr);                break;
		case output_mask:      printf("%s", netmask);             break;
		case output_cidr_full: printf("%s/%d", network, cidr);    break;
		case output_mask_full: printf("%s/%s", network, netmask); break;
		case output_verbose:   printf("host 1: %s\n"
			                          "host 2: %s\n"
			                          "subnet: %s/%d\n"
			                          "mask:   %s",
			                          ntop(addr[0]), ntop(addr[1]),
			                          network, cidr, netmask);
		                       break;
		default:
			PAIN("(internal error) output_format invalid: %d", o.output_format);
			break;
	}

	if (isatty(1))
		putchar('\n');

	free(network);
	free(netmask);
}

int
main(int argc, char **argv)
{
	/* ip_t = uint32_t */
	ip_t addr[2], mask;

	struct options o = {
		.output_format = output_cidr_full,
		.addr[0] = &addr[0],
		.addr[1] = &addr[1]
	};

	setlocale(LC_ALL, "");
	argv0 = argv[0];

	do_getopt(&o, argc, argv);
	ip_sort(addr);

	mask = (ip_t)-1; /* 255.255.255.255 */

	/* find lowest common subnet */
	while ((addr[0] & mask) != (addr[1] & mask))
		mask <<= 1;

	do_output(o, addr, mask);

	return 0;
}
