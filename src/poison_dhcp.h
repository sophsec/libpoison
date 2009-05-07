#ifndef _LIBPOISON_DHCP_
#define _LIBPOISON_DHCP_

/* The different actions we can take with DHCP attacks */
typedef enum
{
	DHCP_HIJACK_DISCOVER, /* "race" to respond to discovers      */
	DHCP_HIJACK_REQUEST,  /* "race" to respond to requests       */
	DHCP_HIJACK_BOTH,     /* "race" to both request and discover */
	DHCP_EXHAUST_LEASES,  /* exhaust remaining leases on server  */
	DHCP_SPOOF_RELEASE    /* spoof release from target to server */

} poison_dhcp_strategy_t;

/* structure of options, per target */
typedef struct
{
	poison_dhcp_strategy_t strategy;
	poison_target_t *target;
	poison_dhcp_options *next;
} poison_dhcp_options_t;

#endif
