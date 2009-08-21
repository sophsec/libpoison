#ifndef _LIBPOISON_ERROR_H_
#define _LIBPOISON_ERROR_H_

typedef struct 
{
	int num;
	char *str;
} poison_error_table_t;

/* max error buffer size */
#define POISON_MAX_ERROR		1024

/* no error */
#define POISON_OK				0

/* generic poison error */
#define POISON_ERR				-1

/* null session passed! */
#define POISON_SESSION_NULL		-2

/* initialized */
#define POISON_INITIALIZED		0x31337

/* not intialized */
#define POISON_NOT_INITIALIZED	-5

/* libnet error */
#define POISON_LIBNET_ERR		-9

/* enomem */
#define POISON_NOMEM			-11

/* state corruption */
#define POISON_CORRUPT			-21

#endif
