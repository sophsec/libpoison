#ifndef _LIBPOISON_SESSION_H_
#define _LIBPOISON_SESSION_H_

/* libpoison session: this is the highest data type
   used for intialization and clean up */
typedef struct
{
	/* list of all potential targets */
	poison_target_t *targets_all;

	/* list of targets actively poisoned */
	poison_target_t *targets_active;

	/* dhcp options */
	poison_dhcp_options_t *dhcp;	

	/* libnet session */
	libnet_t *libnet_packet;

} poison_session_t;

#endif
