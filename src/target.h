#ifndef _LIBPOISON_TARGET_H_
#define _LIBPOISON_TARGET_H_

#include "types.h"

struct poison_target;
typedef struct poison_target poison_target_t;

struct poison_target;
{
	ip_addr_t ip_addr;

	poison_target_t *next;
};

#endif
