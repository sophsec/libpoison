#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include "libpoison.h"

int main(int argc, char *argv[])
{
	poison_session_t session;
	char device[]="eth1";
	int ret;

	poison_discover_opts_t disc;

	memset((char *)&disc, 0, sizeof(disc));
	memset((char *)&session, 0, sizeof(session));

	ret = poison_init(&session, &device);

	if (ret != POISON_OK)
	{
		printf("initialization failed: %s\n", session.errbuf);
		exit(0);
	}

	ret = poison_check_init(&session);

	if (ret != POISON_ALREADY_INIT)
	{
		printf("check init failed\n");
	}

	disc.txid = 0x01020304;
	memcpy((char *)&disc.clientmac, "\x00\x10\x22\x4b\x1c\x00", 6);
	
	ret = poison_dhcp_discover(&session, &disc);

	if (ret != POISON_OK)
	{
		printf("dhcp discover failed!\n");
		printf("error: %s\n", session.errbuf);
		printf("%s\n", session.libnet_err);
	}
	else
	{
		printf("sent discover!\n");
	}
	
	/* shutdown */
	poison_shutdown(&session);
}

