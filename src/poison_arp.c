#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "libpoison.h"

/*
	default arp function: more advanced requirements can call
	poison_arp_adv() 
*/
int poison_arp(poison_session_t *session, unsigned short op, unsigned char *destMAC, unsigned char *destIP, unsigned char *srcMAC, unsigned char *srcIP) 
{
	/* libnet session for ARP should be initialized already */
	libnet_ptag_t arptag;
	libnet_ptag_t ethertag;
	
	arptag = libnet_build_arp(ARPHERD_ETHER, ETHERTYPE_IP, 6, 4, op, srcMAC, 
				srcIP, destMAC, destIP, NULL, 0, session->arp_packet, arptag);

	if (arptag == -1)
	{
		strcpy(session->errbuf, "libnet_build_arp() failed!\n");
		return POISON_LIBNET_ERR;
	}

	ethertag = libnet_build_ethernet(destMAC, srcMAC, ETHERTYPE_ARP, NULL,
				0, session->arp_packet, ethertag);		
	
	if (ethertag == -1)
	{
		strcpy(session->errbuf, "libnet_build_ethernet() failed!\n");
		return POISON_LIBNET_ERR;
	}

	if (libnet_write(session->arp_packet) == -1)
	{
		strcpy(session->errbuf, "libnet_write() failed!\n");
		return POISON_LIBNET_ERR;
	}

	libnet_clear_packet(session->arp_packet);
		
	return POISON_OK;
}
