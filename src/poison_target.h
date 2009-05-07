#ifndef _LIBPOISON_TARGET_H_
#define _LIBPOISON_TARGET_H_

#include "types.h"

/* turn the struct into a type ;D */
typedef struct poison_target poison_target_t;

/* target information list node */
struct poison_target
{
	/* IP of target */
	ip_addr_t ip_addr;

	/* MAC of target */
	unsigned char mac_addr[6];

	/* strategy to poison this target */
	

	/* status flags of poisoning */
	poison_status_t status;

	/* cross links to attacks associated with this target */
	/* dhcp options for this target */	
	poison_dhcp_options_t *dhcp;

	/* next target in the list */
	poison_target_t *next;
};

#endif
