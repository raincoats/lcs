#include "subnet.h"

typedef struct address {
	char str[16];
	union {
		// in_addr_t ip;
		uint32_t ip;
		uint8_t octet[4];
		struct {
			uint8_t a;
			uint8_t b;
			uint8_t c;
			uint8_t d;
		};
	};
} address;

// https://stackoverflow.com/questions/111928
const char *byte_to_binary(int x)
{
    static char b[9];
    int z;

    b[0] = '\0';
    for (z=128; z>0; z>>=1)
        strcat(b, ((x&z)==z)? "1" : "0");

    return b;
}


// in_addr_t
// pton(char *src)
// {
// 	in_addr_t dst;
// 	uint8_t a, b, c, d;

// 	buf = strdup(src);



// 	free(buf);
// 	return dst;
// }

// char *
// ntop(in_addr_t addr)
// {
// 	char buf[16] = { 0 };
// 	char *r = (char *)inet_ntop(AF_INET, &addr, buf, sizeof(buf));
// 	if (r != NULL)
// 		return strdup(r);
// 	else
// 		DIE("inet_ntop returned a null pointer, buf='%s'", buf);
// }

char *
str_from_ip(uint32_t ip)
{
	char *ret;
	uint8_t octet[4];
	int i;

	for (i=0; i<4; i++)
	{
		octet[i] = ip;
		// DEBUG("ip=%#010x, octet %d: %u", ip, i, octet[i]);
		ip >>= 8;
	}

	asprintf(&ret, "%u.%u.%u.%u", octet[3], octet[2], octet[1], octet[0]);
	return ret;
}

uint32_t
ip_from_str(char *orig_str)
{
	uint32_t ip = 0;
	char *str = strdup(orig_str);
	int i;

	if (str == NULL)
		DIE("%s: str == NULL, orig_str='%s'", __func__, orig_str);

		/*
		 *  each octet is added to the uint32_t ip address, and then shifted.
		 *  so if the ip is 1.2.3.4, we add 1 to it, shift it up an octet, add 2
		 *  to the ip, shift that up another octet, etc.
		 *  
		 *  so this loop ends up looking like 
		 *     ip = 0.0.0.1
		 *     ip = 0.0.1.2
		 *     ip = 0.1.2.3
		 *     ip = 1.2.3.4
		*/
	for (i=0; i<4; i++)
	{
		char *s = strsep(&str, ".");
		if (s == NULL)
			PAIN("%s: malformed ip: '%s'", __func__, str);

		int tmp = atoi(s);
		if (tmp > 255 || tmp < 0)
			PAIN("%s: malformed ip (octet %d is not in range 0..255)", __func__, tmp);

		ip <<= 8;
		ip += (uint8_t)tmp;
		// DEBUG("ip=%#010x, octet %d: %s", ip, i, s);
	}

	free(str);
	// DEBUG("ip_from_str: %#010x", ip);
	return ip;
}


struct address
make_address(char *addr)
{
	struct address r;

	memset(&r, 0, sizeof(address));
	strncpy(r.str, addr, sizeof(r.str));
	
	r.ip = ip_from_str(r.str);
	// DEBUG("'%s' -> '%#010x'", r.str, r.ip);
	// DEBUG("'%s'", str_from_ip(r.ip));

	// _exit(3);

	return r;
}


int
cidr_from_mask(uint32_t mask)
{
	int cidr = 0;
	int i = sizeof(uint32_t) * 8;

	/*
	 * while bits are set, cidr++
	 * so for 0b111100000000 cidr would be 4
	 */
	while ((mask >> --i) & 1)
		cidr++;

	return cidr;
}


int
main(int argc, char **argv)
{
	struct address addr0, addr1, mask;

	if (argc < 3)
		DIE("not enough args");

	strncpy(addr0.str, "4.0.0.2", sizeof(addr0.str));
	strncpy(addr1.str, "4.0.0.9", sizeof(addr1.str));
	strncpy(mask.str, "255.255.255.255", sizeof(mask.str));
	// addr0 = pton("4.0.0.2");
	// addr1 = pton("4.0.0.9");
	// addr0 = htonl(addr0);
	// addr1 = htonl(addr1);
	// pton("255.255.255.0", &mask);
	// mask = (in_addr_t)-1;

	// addr0 = make_address("4.0.0.2");
	// addr1 = make_address("4.0.0.9");
	addr0 = make_address(argv[1]);
	addr1 = make_address(argv[2]);
	mask  = make_address("255.255.255.255");

	// DEBUG("addr0=%#010x, addr1=%#010x, mask=%#010x", addr0.ip, addr1.ip, mask.ip);
	// _exit(2);
	// while (addr0)
	while ((addr0.ip & mask.ip) != (addr1.ip & mask.ip))
	{
		if (mask.ip == 0)
			PAIN("mask.ip = 0");
		// DEBUG("%#010x != %#010x (%#010x)", (addr0.ip & mask.ip), (addr1.ip & mask.ip), mask.ip);
		mask.ip <<= 1;
		// mask = htonl(mask << 1);
	}

	uint32_t network_start = (addr0.ip < addr1.ip)? addr0.ip : addr1.ip;

	// GOOD("'%s' and '%s' are in subnet '%s'", addr0.str, addr1.str, str_from_ip(mask.ip));
	GOOD("%s -> %s, subnet=%s", addr0.str, addr1.str, str_from_ip(mask.ip));
	GOOD("%s/%d", str_from_ip(network_start & mask.ip), cidr_from_mask(mask.ip));
/*
	char *buf0 = ntop(addr0.ip);
	char *buf1 = ntop(addr1.ip);
	char *bufm = ntop(mask.ip);
	GOOD("'%s' and '%s' are in subnet '%s'", buf0, buf1, bufm);
	DEBUG("addr0=%#010x, addr1=%#010x, mask=%#010x", addr0.ip, addr1.ip, mask.ip);
*/
	// if ((addr0 & mask) == (addr1 & mask)) {
	// 	GOOD("same subnet");
	// }

	return 0;
}
