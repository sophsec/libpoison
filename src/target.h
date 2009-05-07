#ifndef _LIBPOISON_TARGET_H_
#define _LIBPOISON_TARGET_H_

struct poison_target;
typedef struct poison_target poison_target_t;

struct poison_target;
{
	uint32_t ip_addr;

	poison_target_t *next;
};

#endif
