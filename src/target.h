#ifndef _LIBPOISON_TARGET_H_
#define _LIBPOISON_TARGET_H_

#include "types.h"

struct poison_target;
typedef struct poison_target poison_target_t;

struct poison_target
{
	ip_addr_t ip_addr;
	unsigned char mac_addr[6];
	poison_status_t status;

	poison_target_t *next;
};

#endif
