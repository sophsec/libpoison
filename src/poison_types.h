#ifndef _LIBPOISON_TYPES_H_
#define _LIBPOISON_TYPES_H_

/* maximum error length */
#define POISON_MAX_ERROR 1024

/* status flag for initialized */
#define POISON_INITIALIZED 0x31337

/* error codes */
/* NULL session was passed to function */
#define POISON_SESSION_NULL -2

/* session has already been initialized */
#define POISON_ALREADY_INIT -5

/* session has not been initialized */
#define POISON_NOT_INIT		-7

/* libnet returned an error */
#define POISON_LIBNET_ERR	-9

/* everything is OK */
#define POISON_OK			0

typedef ip_addr_t uint32_t;

/* the strategy used to gain MITM;
   these aren't necessarily mutually exclusive */
typedef enum
{
	POISON_ARP,  /* ARP poisoning */
	POISON_DHCP, /* DHCP forgery */
	POISON_DNS   /* DNS forgery  */
} poison_strategy_t;

/* this is the status flag type: what strategies have we used? */
typedef poison_status_t uint32_t;

/* status flags */
#define LP_STATUS_ARP  0x01 /* ARP poisoning being performed on target */
#define LP_STATUS_DHCP 0x02 /* DHCP forgery being performed on target */
#define LP_STATUS_DNS  0x04 /* DNS forgery being performed on target */


/* state data for future use: resuming state prior to poisoning */
typedef struct
{
	ip_addr_t ip_addr;
	unsigned char mac_addr[6];
	poison_state_t *next;

} poison_state_t;


#endif
