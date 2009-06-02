#ifndef _LIBPOISON_DHCP_
#define _LIBPOISON_DHCP_

/* The different actions we can take with DHCP attacks */
typedef enum
{
	DHCP_NONE,				/* no action 						   */
	DHCP_HIJACK_DISCOVER, 	/* "race" to respond to discovers      */
	DHCP_HIJACK_REQUEST,  	/* "race" to respond to requests       */
	DHCP_HIJACK_BOTH,     	/* "race" to both request and discover */
	DHCP_EXHAUST_LEASES  	/* exhaust remaining leases on server  */
/*
	DHCP_SPOOF_RELEASE  */  /* spoof release from target to server */

} poison_dhcp_strategy_t;



typedef struct
{
	dhcp_xid_t txid;
	/* client MAC is 16 bytes per old bootp standard */
	unsigned char clientmac[16];
	ip_addr_t clientip;
	char hostname[256];

} poison_discover_opts_t;

typedef struct
{
	dhcp_xid_t txid;
	
	/* this is the IP being offered, 
	   it may or may not be the same as the client IP
	   such as in cases where a client already has an IP
	   but will be offered a different one */
	ip_addr_t offeredip;

	/* client ip: this is likely to be 255.255.255.255 
	   unless the client already has an IP */
	ip_addr_t clientip;

	/* IP of the gateway */
	ip_addr_t routerip;
	
	/* IP of the dhcp server */
	ip_addr_t serverip;

	/* netmask */
	ip_addr_t netmask;
	
	/* dns server */
	ip_addr_t dns;
		
	/* domain name */
	char domain[256];
	
	/* lease time */
	size_t leasetime;

	unsigned char servermac[6];
	unsigned char clientmac[6];
} poison_offer_opts_t;

typedef struct
{
	dhcp_xid_t txid;
	
	ip_addr_t ackedip;

	/* client ip: this is likely to be 255.255.255.255 
	   unless the client already has an IP */
	ip_addr_t clientip;

	/* IP of the gateway */
	ip_addr_t routerip;
	
	/* IP of the dhcp server */
	ip_addr_t serverip;

	/* netmask */
	ip_addr_t netmask;
	
	/* dns server */
	ip_addr_t dns;
		
	/* domain name */
	char domain[256];
	
	/* lease time */
	size_t leasetime;

	unsigned char servermac[6];
	unsigned char clientmac[6];
} poison_ack_opts_t;

typedef struct
{
	dhcp_xid_t txid;
	ip_addr_t clientip;
	/* client MAC is 16 bytes per old bootp standard */
	unsigned char clientmac[16];
	ip_addr_t serverip;	
	ip_addr_t servermac[6];

} poison_release_opts_t;


typedef struct 
{
	dhcp_xid_t txid;
	ip_addr_t clientip;
	ip_addr_t serverip;
	
	/* client MAC is 16 bytes per old bootp standard */
	unsigned char clientmac[16];
	char hostname[256];
	char fqdn[256];
} poison_request_opts_t;


#endif
