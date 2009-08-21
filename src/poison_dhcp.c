#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "libpoison.h"


/* Awesome reference page:
http://www.networksorcery.com/enp/protocol/dhcp.htm

interesting:
http://support.microsoft.com/kb/172408
*/

/*
	default dhcp_request function: more advanced requirements can call
	poison_dhcp_request_adv() - once it gets written FIXME
*/
int poison_dhcp_discover(poison_session_t *session, poison_discover_opts_t *options)
{
	/* the options passed in are used to build protocol structured options*/
	unsigned char opts[1024];
	unsigned char namelen = 0;
	size_t optlen = 0;

	/* libnet session for DHCP should be initialized already */
	libnet_ptag_t dhcp;
	libnet_ptag_t udp;
	libnet_ptag_t ip;
	libnet_ptag_t ether;

	/* clean memory */
	memset(opts, 0, sizeof(opts));

	/* build options! */
	opts[optlen++] = 0x35; /* 53 (0x35) specified DHCP */
	opts[optlen++] = 0x01; /* 1 byte length */
	opts[optlen++] = 0x01; /* DHCP discover */

	opts[optlen++] = 0x74; /* DHCP auto config */	
	opts[optlen++] = 0x01; /* 1 byte length */
	opts[optlen++] = 0x01; /* idk? */
	
	/* client identifier: for simplicity, this will be the client mac */
	opts[optlen++] = 0x3d; /* client identifier */
	opts[optlen++] = 0x07; /* 7 byte length */
	opts[optlen++] = 0x01; /* Ethernet */	
	memcpy(&opts[optlen], options->clientmac, 6);
	optlen += 6;

	if (options->clientip != 0)
	{
		opts[optlen++] = 0x32; /* requested IP */
		opts[optlen++] = 0x04; /* addr len */
		memcpy(&opts[optlen], (char *)&options->clientip, 4);
		optlen += 4;
	}

	if (options->hostname[0] != 0)
	{
		/* blah blah demotion of value blah */
		namelen = strlen(options->hostname);
		opts[optlen++] = 0x0c;    /* Host name */
		opts[optlen++] = namelen; /* length */
		strncpy((char *)&opts[optlen], options->hostname, namelen);
		optlen += namelen;
	}

	opts[optlen++] = 0x37; /* parameter request list */
	opts[optlen++] = 0x05; /* list length */
	opts[optlen++] = 0x01; /* subnet mask */
	opts[optlen++] = 0x0f; /* domain name */
	opts[optlen++] = 0x03; /* router */
	opts[optlen++] = 0x1f; /* perform router discover */
	opts[optlen++] = 0x21; /* static route */

	/* end options */
	opts[optlen++] = 0xff;

	/* padding */
	opts[optlen++] = 0x00;
	opts[optlen++] = 0x00;
	opts[optlen++] = 0x00;

	
	/* build libnet packet */
	dhcp = libnet_build_dhcpv4(
				1,					 /* discover == request*/
				DHCP_HTYPE_ETH,		 /* hw: ethernet */
				DHCP_HLEN_ETH,		 /* hw len: 6  */
				DHCP_HOPS_NONE, 	 /* hops: none? idk? */
				options->txid,		 /* transaction id */
				0,					 /* seconds: idk? FIXME */
				0,					 /* flags: */
				0,			 		 /* client ip */
				0,			 		 /* your ip */
				0,			 		 /* server IP */
				0,			 		 /* gateway IP */
				options->clientmac,  /* client hardware addr */
				NULL, 				 /* server hostname */
				NULL, 				 /* file? wtf bbq */
				(u_int8_t *)&opts,	 /* dhcp options */	
				optlen, 			 /* length of options */
				session->dhcp_packet,/* libnet context */
				0					 /* ptag */
			);

	if (dhcp == -1)
	{
		strcpy(session->errbuf, "libnet_build_dhcpv4() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	udp = libnet_build_udp(
			68, 					/* source port */
			67, 					/* dest port */
			LIBNET_UDP_H + 
			LIBNET_DHCPV4_H + 
			optlen,					/* len */
			0, 						/* auto magic checksum */
			NULL, 					/* no payload */
			0,   					/* no payload len */
			session->dhcp_packet,	/* libnet context */
			0);

	if (udp == -1)
	{
		strcpy(session->errbuf, "libnet_build_udp() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	ip = libnet_build_ipv4(
				LIBNET_IPV4_H + 
				LIBNET_UDP_H + 
				LIBNET_DHCPV4_H +
				optlen, 			  /* len */
				0x10,				  /* tos */
				0,					  /* ip id */
				0,					  /* fragmentation */
				16,					  /* ttl */
				IPPROTO_UDP,		  /* upper protocol */
				0,					  /* auto checksum */
				IP_ZERO_ADDR,		  /* src IP */
				IP_BCAST_ADDR, 		  /* dst IP */
				NULL,				  /* no payload */
				0, 					  /* no payload len */
				session->dhcp_packet, /* libnet context */
				0);

	if (ip == -1)
	{
		strcpy(session->errbuf, "libnet_build_ipv4() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	ether = libnet_build_ethernet(
				ETHER_BCAST_ADDR,			/* dest MAC: bcast */	
				options->clientmac,			/* source MAC */
				ETHERTYPE_IP,				/* upper proto */
				NULL,						/* no payload */
				0,							/* no payload len */
				session->dhcp_packet, 		/* context */
				0);
				
	if (ether == -1)
	{
		strcpy(session->errbuf, "libnet_build_ethernet() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}
	
	if (libnet_write(session->dhcp_packet) == -1)
	{
		strcpy(session->errbuf, "libnet_write() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}	

	libnet_clear_packet(session->dhcp_packet);

	return POISON_OK;
}
/* end of poison_dhcp_discover */

int poison_dhcp_release(poison_session_t *session, poison_release_opts_t *options)
{
	/* the options passed in are used to build protocol structured options*/
	unsigned char opts[1024];
	size_t optlen = 0;

	/* libnet session for DHCP should be initialized already */
	libnet_ptag_t dhcp;
	libnet_ptag_t udp;
	libnet_ptag_t ip;
	libnet_ptag_t ether;

	/* clean memory */
	memset(opts, 0, sizeof(opts));

	/* build options! */
	opts[optlen++] = 0x35; /* 53 (0x35) specified DHCP */
	opts[optlen++] = 0x01; /* 1 byte length */
	opts[optlen++] = 0x07; /* DHCP release */

	opts[optlen++] = 0x36; /* server identifier */	
	opts[optlen++] = 0x04; /* server addr length */
	memcpy(&opts[optlen], (char *)&options->serverip, 4);
	optlen += 4;
	
	opts[optlen++] = 0x3d;	/* client identifier */
	opts[optlen++] = 0x07;	/* length of identifier */
	opts[optlen++] = 0x01;  /* Ethernet */	
	memcpy(&opts[optlen], options->clientmac, 6);
	optlen += 6;

	/* end options */
	opts[optlen++] = 0xff;

	/* padding */
	opts[optlen++] = 0x00;
	opts[optlen++] = 0x00;
	opts[optlen++] = 0x00;

	
	/* build libnet packet */
	dhcp = libnet_build_dhcpv4(
				1,					 /* release == request */
				DHCP_HTYPE_ETH,		 /* hw: ethernet */
				DHCP_HLEN_ETH,		 /* hw len: 6  */
				DHCP_HOPS_NONE, 	 /* hops: none? idk? */
				options->txid,		 /* transaction id */
				0,					 /* seconds: idk? FIXME */
				0,					 /* flags: */
				options->clientip,	 /* client ip */
				0,			 		 /* your ip */
				0,			 		 /* server IP */
				0,			 		 /* gateway IP */
				options->clientmac,  /* client hardware addr */
				NULL, 				 /* server hostname */
				NULL, 				 /* file? wtf bbq */
				(u_int8_t *)&opts,	 /* dhcp options */	
				optlen, 			 /* length of options */
				session->dhcp_packet,/* libnet context */
				0					 /* ptag */
			);

	if (dhcp == -1)
	{
		strcpy(session->errbuf, "libnet_build_dhcpv4() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	udp = libnet_build_udp(
			68, 					/* source port */
			67, 					/* dest port */
			LIBNET_UDP_H + 
			LIBNET_DHCPV4_H + 
			optlen,					/* len */
			0, 						/* auto magic checksum */
			NULL, 					/* no payload */
			0,   					/* no payload len */
			session->dhcp_packet,	/* libnet context */
			0);

	if (udp == -1)
	{
		strcpy(session->errbuf, "libnet_build_udp() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	ip = libnet_build_ipv4(
				LIBNET_IPV4_H + 
				LIBNET_UDP_H + 
				LIBNET_DHCPV4_H +
				optlen, 			  /* len */
				0x10,				  /* tos */
				0,					  /* ip id */
				0,					  /* fragmentation */
				16,					  /* ttl */
				IPPROTO_UDP,		  /* upper protocol */
				0,					  /* auto checksum */
				options->clientip,	  /* src IP */
				options->serverip,	  /* dst IP */
				NULL,				  /* no payload */
				0, 					  /* no payload len */
				session->dhcp_packet, /* libnet context */
				0);

	if (ip == -1)
	{
		strcpy(session->errbuf, "libnet_build_ipv4() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	ether = libnet_build_ethernet(
				(u_int8_t *)&options->servermac,/* dest MAC: server */	
				(u_int8_t *)&options->clientmac,/* source MAC */
				ETHERTYPE_IP,			/* upper proto */
				NULL,					/* no payload */
				0,						/* no payload len */
				session->dhcp_packet, 	/* context */
				0);
				
	if (ether == -1)
	{
		strcpy(session->errbuf, "libnet_build_ethernet() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}
	
	if (libnet_write(session->dhcp_packet) == -1)
	{
		strcpy(session->errbuf, "libnet_write() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}	

	libnet_clear_packet(session->dhcp_packet);

	return POISON_OK;
}
/* end of poison_dhcp_release() */

int poison_dhcp_offer(poison_session_t *session, poison_offer_opts_t *options)
{
	/* the options passed in are used to build protocol structured options*/
	unsigned char opts[1024];
	size_t optlen = 0;
	unsigned char domainlen = 0;

	/* libnet session for DHCP should be initialized already */
	libnet_ptag_t dhcp;
	libnet_ptag_t udp;
	libnet_ptag_t ip;
	libnet_ptag_t ether;

	/* clean memory */
	memset(opts, 0, sizeof(opts));

	/* build options! */
	opts[optlen++] = 0x35; /* 53 (0x35) specified DHCP */
	opts[optlen++] = 0x01; /* 1 byte length */
	opts[optlen++] = 0x02; /* DHCP offer */

	opts[optlen++] = 0x36; /* server identifier */	
	opts[optlen++] = 0x04; /* server addr length */
	memcpy(&opts[optlen], (char *)&options->serverip, 4);
	optlen += 4;
	
	opts[optlen++] = 0x33;	/* lease time */
	opts[optlen++] = 0x04;	/* value len  */
	memcpy(&opts[optlen], (char *)&options->leasetime, 4);
	optlen += 4;

	if (options->netmask != 0)
	{
		opts[optlen++] = 0x01; /* netmask */
		opts[optlen++] = 0x04; /* value len */
		memcpy(&opts[optlen], (char *)&options->netmask, 4);
		optlen += 4;
	}

	if (options->domain[0] != 0)
	{
		/* blah yea yea demotion poo */
		domainlen = strlen(options->domain);
		opts[optlen++] = 0x0f; /* domain */
		opts[optlen++] = domainlen;	
		memcpy(&opts[optlen], options->domain, domainlen);
		optlen += domainlen;
	}

	if (options->routerip != 0)
	{
		opts[optlen++] = 0x03; /* router IP */
		opts[optlen++] = 0x04; /* value len */
		memcpy(&opts[optlen], (char *)&options->routerip, 4);
		optlen += 4;
	}

	if (options->dns != 0)
	{
		opts[optlen++] = 0x06; /* dns IP */
		opts[optlen++] = 0x04; /* value len */
		memcpy(&opts[optlen], (char *)&options->dns, 4);
		optlen += 4;
	}

	/* end options */
	opts[optlen++] = 0xff;

	/* padding */
	opts[optlen++] = 0x00;
	opts[optlen++] = 0x00;
	opts[optlen++] = 0x00;

	/* build libnet packet */
	dhcp = libnet_build_dhcpv4(
				2,					 /* offer == reply */
				DHCP_HTYPE_ETH,		 /* hw: ethernet */
				DHCP_HLEN_ETH,		 /* hw len: 6  */
				DHCP_HOPS_NONE, 	 /* hops: none? idk? */
				options->txid,		 /* transaction id */
				0,					 /* seconds: idk? FIXME */
				0,					 /* flags: */
				0,			 		 /* client ip */
				/* this is intentional however it does */
				/* get confuzing with your and client ip */
				options->offeredip,	 /* your ip */
				options->serverip,	 /* server IP */
				0,			 		 /* gateway IP */
				options->clientmac,  /* client hardware addr */
				NULL, 				 /* server hostname */
				NULL, 				 /* file? wtf bbq */
				(u_int8_t *)&opts,	 /* dhcp options */	
				optlen, 			 /* length of options */
				session->dhcp_packet,/* libnet context */
				0					 /* ptag */
			);

	if (dhcp == -1)
	{
		strcpy(session->errbuf, "libnet_build_dhcpv4() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	udp = libnet_build_udp(
			67, 					/* source port */
			68, 					/* dest port */
			LIBNET_UDP_H + 
			LIBNET_DHCPV4_H + 
			optlen,					/* len */
			0, 						/* auto magic checksum */
			NULL, 					/* no payload */
			0,   					/* no payload len */
			session->dhcp_packet,	/* libnet context */
			0);

	if (udp == -1)
	{
		strcpy(session->errbuf, "libnet_build_udp() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	ip = libnet_build_ipv4(
				LIBNET_IPV4_H + 
				LIBNET_UDP_H + 
				LIBNET_DHCPV4_H +
				optlen, 			  /* len */
				0x10,				  /* tos */
				0,					  /* ip id */
				0,					  /* fragmentation */
				16,					  /* ttl */
				IPPROTO_UDP,		  /* upper protocol */
				0,					  /* auto checksum */
				options->serverip,	  /* src IP */
				options->clientip,	  /* dst IP */
				NULL,				  /* no payload */
				0, 					  /* no payload len */
				session->dhcp_packet, /* libnet context */
				0);

	if (ip == -1)
	{
		strcpy(session->errbuf, "libnet_build_ipv4() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	ether = libnet_build_ethernet(
				options->clientmac,		/* dest MAC: bcast */	
				options->servermac,		/* source MAC */
				ETHERTYPE_IP,			/* upper proto */
				NULL,					/* no payload */
				0,						/* no payload len */
				session->dhcp_packet, 	/* context */
				0);
				
	if (ether == -1)
	{
		strcpy(session->errbuf, "libnet_build_ethernet() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}
	
	if (libnet_write(session->dhcp_packet) == -1)
	{
		strcpy(session->errbuf, "libnet_write() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}	

	libnet_clear_packet(session->dhcp_packet);

	return POISON_OK;
}
/* end of poison_dhcp_offer */

int poison_dhcp_request(poison_session_t *session, poison_request_opts_t *options)
{
	/* the options passed in are used to build protocol structured options*/
	unsigned char opts[1024];
	size_t optlen = 0;
	unsigned char domainlen = 0;
	unsigned char namelen = 0;

	/* libnet session for DHCP should be initialized already */
	libnet_ptag_t dhcp;
	libnet_ptag_t udp;
	libnet_ptag_t ip;
	libnet_ptag_t ether;

	/* clean memory */
	memset(opts, 0, sizeof(opts));

	/* build options! */
	opts[optlen++] = 0x35; /* 53 (0x35) specified DHCP */
	opts[optlen++] = 0x01; /* 1 byte length */
	opts[optlen++] = 0x03; /* DHCP request */

	opts[optlen++] = 0x3d;	/* client identifier */
	opts[optlen++] = 0x07;	/* length of identifier */
	opts[optlen++] = 0x01;  /* Ethernet */	
	memcpy(&opts[optlen], options->clientmac, 6);
	optlen += 6;

	/* Requested IP */
	opts[optlen++] = 0x30; 
	opts[optlen++] = 0x04; /* value len */
	memcpy(&opts[optlen], (char *)&options->clientip, 4);
	optlen += 4;

	if (options->serverip != 0)
	{
		opts[optlen++] = 0x36;
		opts[optlen++] = 0x04;
		memcpy(&opts[optlen], (char *)&options->serverip, 4);
		optlen += 4;
	}

	if (options->hostname[0] != 0)
	{
		/* blah blah demotion of value blah */
		namelen = strlen(options->hostname);
		opts[optlen++] = 0x0c;    /* Host name */
		opts[optlen++] = namelen; /* length */
		strncpy((char *)&opts[optlen], options->hostname, namelen);
		optlen += namelen;
	}


	if (options->fqdn[0] != 0)
	{
		domainlen = strlen(options->fqdn);
		domainlen += 3;			  /* flags, A-PR, PTR-PR */
		opts[optlen++] = 0x51;    /* Host name */
		opts[optlen++] = domainlen; /* length */
		opts[optlen++] = 0x0;	  /* flags */
		opts[optlen++] = 0x0;	  /* A-PR */
		opts[optlen++] = 0x0;     /* PTR-PR */
		strncpy((char *)&opts[optlen], options->fqdn, domainlen);
		optlen += domainlen;
	}


	opts[optlen++] = 0x37; /* parameter request list */
	opts[optlen++] = 0x05; /* list length */
	opts[optlen++] = 0x01; /* subnet mask */
	opts[optlen++] = 0x0f; /* domain name */
	opts[optlen++] = 0x03; /* router */
	opts[optlen++] = 0x1f; /* perform router discover */
	opts[optlen++] = 0x21; /* static route */


	/* end options */
	opts[optlen++] = 0xff;

	/* padding */
	opts[optlen++] = 0x00;
	opts[optlen++] = 0x00;
	opts[optlen++] = 0x00;

	/* build libnet packet */
	dhcp = libnet_build_dhcpv4(
				1,					 /* request == request */
				DHCP_HTYPE_ETH,		 /* hw: ethernet */
				DHCP_HLEN_ETH,		 /* hw len: 6  */
				DHCP_HOPS_NONE, 	 /* hops: none? idk? */
				options->txid,		 /* transaction id */
				0,					 /* seconds: idk? FIXME */
				0,					 /* flags: */
				0,			 		 /* client ip */
				0,					 /* your ip */
				0,					 /* server IP */
				0,			 		 /* gateway IP */
				options->clientmac,  /* client hardware addr */
				NULL, 				 /* server hostname */
				NULL, 				 /* file? wtf bbq */
				(u_int8_t*)&opts,	 /* dhcp options */	
				optlen, 			 /* length of options */
				session->dhcp_packet,/* libnet context */
				0					 /* ptag */
			);

	if (dhcp == -1)
	{
		strcpy(session->errbuf, "libnet_build_dhcpv4() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	udp = libnet_build_udp(
			68, 					/* source port */
			67, 					/* dest port */
			LIBNET_UDP_H + 
			LIBNET_DHCPV4_H + 
			optlen,					/* len */
			0, 						/* auto magic checksum */
			NULL, 					/* no payload */
			0,   					/* no payload len */
			session->dhcp_packet,	/* libnet context */
			0);

	if (udp == -1)
	{
		strcpy(session->errbuf, "libnet_build_udp() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	ip = libnet_build_ipv4(
				LIBNET_IPV4_H + 
				LIBNET_UDP_H + 
				LIBNET_DHCPV4_H +
				optlen, 			  /* len */
				0x10,				  /* tos */
				0,					  /* ip id */
				0,					  /* fragmentation */
				16,					  /* ttl */
				IPPROTO_UDP,		  /* upper protocol */
				0,					  /* auto checksum */
				0,					  /* src IP */
				IP_BCAST_ADDR,		  /* dst IP */
				NULL,				  /* no payload */
				0, 					  /* no payload len */
				session->dhcp_packet, /* libnet context */
				0);

	if (ip == -1)
	{
		strcpy(session->errbuf, "libnet_build_ipv4() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	ether = libnet_build_ethernet(
				ETHER_BCAST_ADDR,		/* dest MAC: bcast */	
				options->clientmac,		/* source MAC */
				ETHERTYPE_IP,			/* upper proto */
				NULL,					/* no payload */
				0,						/* no payload len */
				session->dhcp_packet, 	/* context */
				0);
				
	if (ether == -1)
	{
		strcpy(session->errbuf, "libnet_build_ethernet() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}
	
	if (libnet_write(session->dhcp_packet) == -1)
	{
		strcpy(session->errbuf, "libnet_write() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}	

	libnet_clear_packet(session->dhcp_packet);

	return POISON_OK;
}
/* end of poison_dhcp_request */


int poison_dhcp_ack(poison_session_t *session, poison_ack_opts_t *options)
{
	/* the options passed in are used to build protocol structured options*/
	unsigned char opts[1024];
	size_t optlen = 0;
	unsigned char domainlen = 0;

	/* libnet session for DHCP should be initialized already */
	libnet_ptag_t dhcp;
	libnet_ptag_t udp;
	libnet_ptag_t ip;
	libnet_ptag_t ether;

	/* clean memory */
	memset(opts, 0, sizeof(opts));

	/* build options! */
	opts[optlen++] = 0x35; /* 53 (0x35) specified DHCP */
	opts[optlen++] = 0x01; /* 1 byte length */
	opts[optlen++] = 0x05; /* DHCP ack */

	opts[optlen++] = 0x36; /* server identifier */	
	opts[optlen++] = 0x04; /* server addr length */
	memcpy(&opts[optlen], (char *)&options->serverip, 4);
	optlen += 4;
	
	opts[optlen++] = 0x33;	/* lease time */
	opts[optlen++] = 0x04;	/* value len  */
	memcpy(&opts[optlen], (char *)&options->leasetime, 4);
	optlen += 4;

	if (options->netmask != 0)
	{
		opts[optlen++] = 0x01; /* netmask */
		opts[optlen++] = 0x04; /* value len */
		memcpy(&opts[optlen], (char *)&options->netmask, 4);
		optlen += 4;
	}

	if (options->domain[0] != 0)
	{
		/* blah yea yea demotion poo */
		domainlen = strlen(options->domain);
		opts[optlen++] = 0x0f; /* domain */
		opts[optlen++] = domainlen;	
		memcpy(&opts[optlen], options->domain, domainlen);
		optlen += domainlen;
	}

	if (options->routerip != 0)
	{
		opts[optlen++] = 0x03; /* router IP */
		opts[optlen++] = 0x04; /* value len */
		memcpy(&opts[optlen], (char *)&options->routerip, 4);
		optlen += 4;
	}

	if (options->dns != 0)
	{
		opts[optlen++] = 0x06; /* dns IP */
		opts[optlen++] = 0x04; /* value len */
		memcpy(&opts[optlen], (char *)&options->dns, 4);
		optlen += 4;
	}

	/* end options */
	opts[optlen++] = 0xff;

	/* padding */
	opts[optlen++] = 0x00;
	opts[optlen++] = 0x00;
	opts[optlen++] = 0x00;

	/* build libnet packet */
	dhcp = libnet_build_dhcpv4(
				2,					 /* offer == reply */
				DHCP_HTYPE_ETH,		 /* hw: ethernet */
				DHCP_HLEN_ETH,		 /* hw len: 6  */
				DHCP_HOPS_NONE, 	 /* hops: none? idk? */
				options->txid,		 /* transaction id */
				0,					 /* seconds: idk? FIXME */
				0,					 /* flags: */
				0,			 		 /* client ip */
				/* this is intentional however it does */
				/* get confuzing with your and client ip */
				options->ackedip,	 /* your ip */
				options->serverip,	 /* server IP */
				0,			 		 /* gateway IP */
				options->clientmac,  /* client hardware addr */
				NULL, 				 /* server hostname */
				NULL, 				 /* file? wtf bbq */
				(u_int8_t *)&opts,	 /* dhcp options */	
				optlen, 			 /* length of options */
				session->dhcp_packet,/* libnet context */
				0					 /* ptag */
			);

	if (dhcp == -1)
	{
		strcpy(session->errbuf, "libnet_build_dhcpv4() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	udp = libnet_build_udp(
			67, 					/* source port */
			68, 					/* dest port */
			LIBNET_UDP_H + 
			LIBNET_DHCPV4_H + 
			optlen,					/* len */
			0, 						/* auto magic checksum */
			NULL, 					/* no payload */
			0,   					/* no payload len */
			session->dhcp_packet,	/* libnet context */
			0);

	if (udp == -1)
	{
		strcpy(session->errbuf, "libnet_build_udp() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	ip = libnet_build_ipv4(
				LIBNET_IPV4_H + 
				LIBNET_UDP_H + 
				LIBNET_DHCPV4_H +
				optlen, 			  /* len */
				0x10,				  /* tos */
				0,					  /* ip id */
				0,					  /* fragmentation */
				16,					  /* ttl */
				IPPROTO_UDP,		  /* upper protocol */
				0,					  /* auto checksum */
				options->serverip,	  /* src IP */
				options->clientip,	  /* dst IP */
				NULL,				  /* no payload */
				0, 					  /* no payload len */
				session->dhcp_packet, /* libnet context */
				0);

	if (ip == -1)
	{
		strcpy(session->errbuf, "libnet_build_ipv4() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}

	ether = libnet_build_ethernet(
				options->clientmac,		/* dest MAC: bcast */	
				options->servermac,		/* source MAC */
				ETHERTYPE_IP,			/* upper proto */
				NULL,					/* no payload */
				0,						/* no payload len */
				session->dhcp_packet, 	/* context */
				0);
				
	if (ether == -1)
	{
		strcpy(session->errbuf, "libnet_build_ethernet() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}
	
	if (libnet_write(session->dhcp_packet) == -1)
	{
		strcpy(session->errbuf, "libnet_write() failed\n");
		strncpy(session->libnet_err, 
			libnet_geterror(session->dhcp_packet), sizeof(session->libnet_err)-1);
		return POISON_LIBNET_ERR;
	}	

	libnet_clear_packet(session->dhcp_packet);

	return POISON_OK;
}
