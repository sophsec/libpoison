#ifndef _LIBPOISON_TRIGGER_H_
#define _LIBPOISON_TRIGGER_H_

#include "poison_host.h"

typedef struct poison_trigger poison_trigger_t;

struct poison_trigger
{
	/* host information for the trigger */
	poison_host_t host;

	/* next trigger */
	poison_trigger_t *next;
};

#endif
