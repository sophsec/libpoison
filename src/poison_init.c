#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "libpoison.h"

/* intialize the session: returns negative on failure, 
   sets errbuf in session, unless session is NULL, in which case
   returns error code reflecting that  */
int poison_init(poison_session_t *session, char *interface)
{
	char libnet_err[LIBNET_ERRBUF_SIZE];

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
	memset(libnet_err, 0, sizeof(libnet_err));

	/* setup libnet */	
	session->arp_packet = libnet_init(LIBNET_LINK, interface, &libnet_err);

	if (!session->arp_packet)
	{
		strcpy(session->errbuf, "libnet_init for ARP failed: ");
		strncpy(session->errbuf, libnet_err, 
				sizeof(session->errbuf) - strlen(session->errbuf) - 1);
		return POISON_LIBNET_ERR;
	}
}

