#ifndef _LIBPOISON_SESSION_H_
#define _LIBPOISON_SESSION_H_

/* libpoison session: this is the highest data type
   used for intialization and clean up */
typedef struct
{
	/* error reporting buffer */
	char errbuf[POISON_MAX_ERROR];
	char libnet_err[LIBNET_ERRBUF_SIZE];

	/* initialized flag */
	unsigned int initialized;

	/* list of all potential targets */
	poison_target_t *targets_all;

	/* list of targets actively poisoned */
	poison_target_t *targets_active;

	/* dhcp options */
	poison_dhcp_options_t *dhcp;	

	/* libnet sessions
	   each attack has they reside at different layers */
	libnet_t *dhcp_packet;
	libnet_t *arp_packet;
	libnet_t *dns_packet;

} poison_session_t;

#endif
