
/* code to read, update routing tables on linux */

#include "poison_route.h"
#include "libpoison.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int poison_get_route(poison_session_t *session)
{
    int fd;
	char buf[200];
	ssize_t rbytes;


	/* first, before clearing old route info (assuming there is any)
	   verify the file could be opened to read! */
    fd = open(PROCIPFORWARD, O_RDONLY);

    if (fd == -1)
    {
        return POISON_ERROR;
    }
	
	/* clear any existing route info */
	poison_unlink_route(session);

	/* read in data */
	/* each line should be < 200 bytes */
	
	rbytes = read(fd, buf, sizeof(buf)-1);
	
	/* first line is the line specifying the order of data given 
	   this line should be at least 70 bytes! */
	/* FIXME: will the order ever change? this assumes static order! */

}

/* route node allocation: returns new initialized (to 0) node or NULL on fail */
poison_route_t *poison_route_new(void)
{
	poison_route_t *p = NULL;

	/* allocate */
	p = malloc(sizeof(poison_route_t));

	/* clean memory if memory was given */
	if (p)
	{
		memset((char *)p, 0, sizeof(poison_route_t));
	}

	/* BLAM */
	return p;
}

void poison_unlink_route(poison_session_t *session)
{
	poison_route_t *p = NULL;
	poison_route_t *n = NULL;
	
	p = session->route;

	while (p)
	{
		n = p->next;
		free(p);
		p = n;
	}

	session->route = NULL;

	return;
}

