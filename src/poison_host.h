#ifndef _LIBPOISON_HOST_H_
#define _LIBPOISON_HOST_H_

#include "poison_types.h"

typedef struct
{
	/* IP  */
	ip_addr_t ip;

	/* MAC */
	unsigned char mac[6];
} poison_host_t;

#endif
