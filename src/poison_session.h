#ifndef _LIBPOISON_SESSION_H_
#define _LIBPOISON_SESSION_H_

/* libpoison session: this is the highest data type
   used for intialization and clean up */
typedef struct
{
	/* list of targets */
	poison_target_t *targets;

	/* dhcp options */
	poison_dhcp_options_t *dhcp;	

} poison_session_t;

#endif
