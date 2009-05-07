#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "libpoison.h"

/* this function registers a new target for dhcp poisoning */
int poison_add_dhcp(poison_session_t *session, poison_target_t *target, dhcp_strategy_t)
{
	

}

/* generic linked list free routine */
void poison_free_dhcp(poison_dhcp_options_t *dhcp)
{
	poison_dhcp_options_t *cur;
	poison_dhcp_options_t *next;

	cur = dhcp;

	while (cur)
	{
		next = cur->next;
		free(cur);
		cur = next;
	}

	return;
}

