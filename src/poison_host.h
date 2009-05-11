#ifndef _LIBPOISON_HOST_H_
#define _LIBPOISON_HOST_H_

#include "poison_types.h"

typedef struct
{
	/* IP of target */
	ip_addr_t ip_addr;

	/* MAC of target */
	unsigned char mac_addr[6];
} poison_host_t;

#endif
