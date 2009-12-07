#ifndef _LIBPOISON_ROUTE_H_
#define _LIBPOISON_ROUTE_H_

#include "libpoison.h"
#define PROCIPROUTE "/proc/net/route"
#define ROUTEHEADER "Iface\tDestination\tGateway \t" \
					"Flags\tRefCnt\tUse\tMetric\tMask\t" \
					"\tMTU\tWindow\tIRTT" \
					"                                       \n"

typedef struct
{
	/* FIXME: what is the max interface name length? */
	char interface[128];

	/* destination */
	ip_addr_t dest;

	/* gateway */
	ip_addr_t gateway;

	/* netmask */
	ip_addr_t netmask;

	/* FIXME: flags == 16 bit? */
	unsigned short flags;

	/* although not currently relevant, 
	   other data provided in file is also given */
	/* FIXME type sizes? */
	unsigned int refcnt; 	
	unsigned int use;
	unsigned int metric;
	unsigned int mtu;
	unsigned int window;
	unsigned int irtt;
	
	/* next node */
	struct poison_route_t *next;
} poison_route_t;

#endif
