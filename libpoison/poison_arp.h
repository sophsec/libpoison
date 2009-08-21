#ifndef _LIBPOISON_ARP_H_
#define _LIBPOISON_ARP_H_

#include "poison_session.h"
#include "poison_types.h"

int poison_arp(poison_session_t *session, unsigned short op, unsigned char *destMAC, ip_addr_t destIP, unsigned char *srcMAC, ip_addr_t srcIP);

#endif
