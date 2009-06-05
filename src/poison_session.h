#ifndef _LIBPOISON_SESSION_H_
#define _LIBPOISON_SESSION_H_

#include <libnet.h>

#include "poison_types.h"
#include "poison_target.h"
#include "poison_route.h"

/* libpoison session: this is the highest data type
   used for intialization and clean up */
typedef struct
{
	/* error reporting buffer */
	char errbuf[POISON_MAX_ERROR];
	char libnet_err[LIBNET_ERRBUF_SIZE];

	/* initialized flag */
	unsigned int initialized;

	/* interface name */
	char interface[64];

	/* list of all targets, owned and potential to own */
	poison_target_t *targets;

	/* list of targets to be owned, awaiting trigger */
	poison_target_t *triggers;


	/* libnet sessions
	   each attack has they reside at different layers */
	libnet_t *dhcp_packet;
	libnet_t *arp_packet;
	libnet_t *dns_packet;

	/* routing table info */
	poison_route_t *route;

} poison_session_t;

#endif
