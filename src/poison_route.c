
/* code to read, update routing tables on linux */

#include "poison_route.h"
#include "libpoison.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* macro GETNEXT is used for parsing /proc/net/route file */
#define GETNEXT (cur, next) {next = cur+1; if (!(cur = strchr(next, 0x09))) goto errout; *cur = 0; }

int poison_get_route(poison_session_t *session)
{
    FILE *fd = NULL;
	char *p1 = NULL, *p2 = NULL;
	char buf[256];
	ssize_t rbytes = 0;
	size_t slen = 0;
	int retval = POISON_OK;
	poison_route_t info;
	poison_route_t *ptr = NULL;
	poison_route_t *last = NULL;

	/* first, before clearing old route info (assuming there is any)
	   verify the file could be opened to read! */
    fd = fopen(PROCIPROUTE, "r");

    if (!fd)
    {
		strcpy(session->errbuf, "unable to open /proc/net/route file!");
        return POISON_ERROR;
    }
	
	/* clear any existing route info */
	poison_unlink_route(session);

	/* read in data */
	/* first line is the line specifying the order of data given 
	   this line should be at least 70 bytes! lol no checks */
	/* FIXME: will the order ever change? this assumes static order! */

	/* throw away first read, just to get to real info */
	fgets(buf, sizeof(buf), fd);

	/* then loop rest of file */
	while (fgets(buf,sizeof(buf),fd))
	{
		/* clean temporary structure */
		memset((char *)&info, 0, sizeof(info));

		/* find the tab delimiter */
		p2 = strchr(buf, 0x09);

		/* not there? break out */
		if (!p2)
		{
			goto errout;
		}
		
		/* terminate the string */
		*p2 = 0;

		/* copy in the interface name */
		strncpy(info.interface, buf, sizeof(info.interface)-1);

		/* move foward */	
		GETNEXT(p2, p1)

		/* read/store destination */
		info.dest = hex2ip(p1);

		/* move forward */
		GETNEXT(p2, p1)

		/* read/store gateway */
		info.gateway = hex2ip(p1);	

		/* move forward */
		GETNEXT(p2, p1)

		/* read/story flags */
		info.flags = atoi(p1);	

		/* move forward */
		GETNEXT(p2, p1)
				
		/* refcount */
		info.refcnt = atoi(p1);

		/* move forward */
		GETNEXT(p2, p1)
				
		info.use = atoi(p1);	

		/* move forward */
		GETNEXT(p2, p1)

		/* metric */
		info.metric = atoi(p1);

		/* move forward */
		GETNEXT(p2, p1)
		
		info.netmask = hex2ip(p1);	
		
		/* move forward */
		GETNEXT(p2, p1)
		
		info.mtu = atoi(p1);		

		/* move forward */
		GETNEXT(p2, p1)
				
		/* window */
		info.window = atoi(p1);	

		/* move forward */
		GETNEXT(p2, p1)

		/* IRTT */
		info.irtt = atoi(p1);

		/* now get a new route node */	
		ptr = poison_new_route();
	
		if (!ptr)
		{
			goto errout;
		}
		
		/* copy in the route info */
		memcpy((char *)ptr, (char *)&info, sizeof(poison_route_t));
		
		/* is it the first to be added to the list? */
		if (!session->route)
		{
			session->route = ptr;
		}
		else
		{
			/* add it to the last pointer */
			last->next = ptr;
		}

		last = ptr;
	}

	goto done;

errout:
	strcpy(session->errbuf, "corrupt /proc/net/route file!");
	poison_unlink_route(session);
	retval = POISON_ERROR;

done:
	fclose(fd);	
	return retval;
}


/* write the route information in the session->route list
	to the /proc/net/route table - overwrite! */
int poison_set_route(poison_session_t *session)
{
	FILE *fd = NULL;

	/* open the route file */
	fd = fopen(PROCIPROUTE, "w");

	if (!fd)
	{
		return POISON_ERROR;
	}
}


/* removes nodes from session->route list, based on matching
	attributes interface, dest and gateway from info */
void poison_del_route(poison_session_t *session, poison_route_t *info)
{
	poison_route_t *local = NULL;
	poison_route_t *temp = NULL;

	local = session->route;

	while (local)
	{
		/* match interface */
		if (strcmp(info.interface, local.interface)) 
		{
			temp = local;
			local = local->next;
			continue;
		}

		/* match dest */
		if (info.dest != local.dest)
		{
			temp = local;
			local = local->next;
			continue;
		}

		/* match gateway */
		if (info.gateway != local.gateway)
		{
			temp = local;
			local = local->next;
			continue;
		}

		/* REMOVE IT! */	

		/* is it the first node? */
		if (local == session->route)
		{
			session->route = local->next;
			free(local);	
		}	
		/* update from the last evaluated node */
		else
		{
			temp->next = local->next;
			free(local);		
			local = temp->next;
		}
	}

	return;
}

/* adds a copy of info route to the front of the session->route list */
int poison_add_route(poison_session_t *session, poison_route_t *info)
{
	poison_route_t *local = NULL;
	poison_route_t *temp = NULL;

	local = poison_route_new();
		
	if (!local)
	{
		strcpy(session->errbuf, "out of memory!\n");
		return POISON_ERROR;
	}

	memcpy((char *)local, (char *)info, sizeof(poison_route_t));

	temp = session->route;
	session->route = local;
	local->next = temp;

	return POISON_OK;
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

/* unlink the session->route linked list */
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

