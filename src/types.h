#ifndef _LIBPOISON_TYPES_H_
#define _LIBPOISON_TYPES_H_

typedef ip_addr_t uint32_t;

typedef enum
{
	POISON_ARP,
	POISON_DHCP,
	POISON_DNS
} poison_strategy_t;

#endif
