#ifndef _LIBPOISON_TYPES_H_
#define _LIBPOISON_TYPES_H_

/* maximum error length */
#define POISON_MAX_ERROR 1024

/* status flag for initialized */
#define POISON_INITIALIZED 0x31337

/* error codes */

/* everything is OK */
#define POISON_OK			0

/* NULL session was passed to function */
#define POISON_SESSION_NULL -2

/* session has already been initialized */
#define POISON_ALREADY_INIT -5

/* session has not been initialized */
#define POISON_NOT_INIT		-7

/* libnet returned an error */
#define POISON_LIBNET_ERR	-9

/* memory error.. */
#define POISON_NOMEM		-11

/* define state corruption */
#define POISON_CORRUPT		-21

/* IP address type */
typedef uint32_t ip_addr_t;

/* DHCP transaction ID */
typedef uint32_t dhcp_xid_t;

/* strategy: how to MITM this target */
typedef uint32_t poison_strategy_t;

/* this is the status flag type: what strategies have we used? */
typedef uint32_t poison_status_t;

/* strategy flags */
#define POISON_DHCP_REQUEST  0x01
#define POISON_DHCP_DISCOVER 0x02
#define POISON_DHCP_BOTH     0x03
#define POISON_ARP_REQUEST   0x04
#define POISON_ALL_NO_DNS    0x07
#define POISON_DNS_REQUEST   0x08
#define POISON_ALL           0x0f

/* DHCP OP FLAGS */ 
#define DHCP_OP_DISCOVER	0x01	
#define DHCP_OP_OFFER		0x02
#define DHCP_OP_REQUEST		0x03
#define DHCP_OP_ACK			0x05
#define DHCP_OP_RELEASE 	0x07

/* DHCP Hardware Len */
#define DHCP_HTYPE_ETH		0x01

/* DHCP Hardware Len */
#define DHCP_HLEN_ETH		0x06

/* DHCP HOPS - possibly look into this in the future */
#define DHCP_HOPS_NONE		0x0


/* status flags */
#define POISON_STATUS_ARP  0x01 /* ARP poisoning being performed on target */
#define POISON_STATUS_DHCP 0x02 /* DHCP forgery being performed on target */
#define POISON_STATUS_DNS  0x04 /* DNS forgery being performed on target */


/* state data for future use: resuming state prior to poisoning */
typedef struct
{
	ip_addr_t ip_addr;
	unsigned char mac_addr[6];
	poison_state_t *next;

} poison_state_t;


#endif
