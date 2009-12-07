#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "libpoison.h"
#include "poison_session.h"

/* intialize the session: returns negative on failure, 
   sets errbuf in session, unless session is NULL, in which case
   returns error code reflecting that  */
int poison_init(poison_session_t *session, const char *interface)
{
	/* No session given? FAIL! */
	if (!session)
	{
		return POISON_SESSION_NULL;
	}

	/* Has it been initialized already? */
	if (POISON_INITIALIZED == session->initialized)
	{
		/* set error buf */
		strcpy(session->errbuf, "libpoison already initialized!");		

		/* alert caller */
		return POISON_ALREADY_INIT;
	}
	
	/* Clean all memory! */
	memset((char *)session, 0, sizeof(poison_session_t));

	strncpy(session->interface, interface, sizeof(session->interface)-1);

	/* setup libnet context for arp */	
	session->arp_packet = libnet_init(LIBNET_LINK, session->interface, session->libnet_err);

	/* verify it worked */
	if (!session->arp_packet)
	{
		strcpy(session->errbuf, "libnet_init for ARP failed");
		return POISON_LIBNET_ERR;
	}

	/* setup libnet context for dhcp */
	session->dhcp_packet = libnet_init(LIBNET_LINK, session->interface, session->libnet_err);

	/* verify it worked */
	if (!session->dhcp_packet)
	{
		strcpy(session->errbuf, "libnet_init for DHCP failed");
		return POISON_LIBNET_ERR;
	}

	/* setup libnet context for dns */
	session->dns_packet = libnet_init(LIBNET_RAW4, session->interface, session->libnet_err);

	/* verify it worked */
	if (!session->dns_packet)
	{
		strcpy(session->errbuf, "libnet_init for DNS failed");
		return POISON_LIBNET_ERR;
	}


	/* set flag */
	session->initialized = SESSION_INITIALIZED;	
	return POISON_OK;
}


/* a check function: are we initialized? */
int poison_check_init(poison_session_t *session)
{
	/* verify session is not NULL */
	if (!session)
	{
		return POISON_SESSION_NULL;
	}

	/* check if initialized */
	if (SESSION_INITIALIZED == session->initialized)
	{
		return POISON_ALREADY_INIT;
	}

	/* not initialized */
	return POISON_NOT_INIT;
}

/* shutdown poison: close libnet / pcap contexts
	free memory lists */
void poison_shutdown(poison_session_t *session)
{
	/* verify session is not NULL */
	if (!session)
	{
		return;
	}

	/* if session is not initialized, fail */
	if (SESSION_INITIALIZED != session->initialized)
	{
		return;
	}

#if 0
	/* free targets_all list */
	if (session->targets_all)
	{
		poison_free_targets(session->targets_all);
	}
	
	/* free targets_waiting list */
	if (session->targets_waiting)
	{
		poison_free_targets(session->targets_waiting);
	}

	/* free dhcp poison options */
	if (poison->dhcp)
	{
		poison_free_dhcp(poison->dhcp);
	}
#endif

	/* close libnet dhcp packet */
	if (session->dhcp_packet)
	{
		libnet_destroy(session->dhcp_packet);
	}

	/* close libnet arp packet */
	if (session->arp_packet)
	{
		libnet_destroy(session->arp_packet);
	}

	/* close libnet dns packet */
	if (session->dns_packet)
	{
		libnet_destroy(session->dns_packet);
	}

	return;
}



