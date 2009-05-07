#ifndef _LIBPOISON_TARGETS_H_
#define _LIBPOISON_TARGETS_H_

#include "target.h"

typedef struct
{
	poison_target_t *head;
	poison_target_t *tail;
} poison_targets_t;

poison_targets_t * poison_targets_create();
int poison_targets_append(const struct poison_target *target);

const poison_target_t * poison_targets_ip(ip_addr_t ip);

void poison_targets_destroy(poison_targets_t *list);

#endif
