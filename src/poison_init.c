#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "libpoison.h"

/* intialize the session: returns negative on failure, 
   sets errbuf in session, unless session is NULL, in which case
   returns error code reflecting that  */
int poison_init(poison_session_t *session)
{
	/* No session given? FAIL! */
	if (!session)
	{
		return POISON_SESSION_NULL;
	}

	/* Has it been initialized already? */
	if (POISON_INITIALIZED == session->initialized)
	{
		/* alert caller */
		return POISON_ALREADY_INIT;
	}
	
	/* Clean all memory! */
	memset((char *)session, 0, sizeof(poison_session_t));

	

}

