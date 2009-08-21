#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <libpoison.h>
#include <poison_init.h>
#include <CUnit/Basic.h>

#include "test_dhcp.h"
#include "test_config.h"

int test_suite_dhcp()
{
	CU_pSuite suite;

	if ((suite=CU_add_suite("dhcp", test_dhcp_init, test_dhcp_cleanup)) == NULL)
	{
		return -1;
	}

	if (CU_add_test(suite, "test poison_dhcp_discover", test_dhcp_discover) == NULL)
	{
		return -1;
	}

	return 0;
}

static poison_session_t session;
static poison_discover_opts_t disc;

int test_dhcp_init()
{
	int ret;

	ret = poison_init(&session, test_device);
	
	if (ret != POISON_OK)
	{
		fprintf(stderr, "initialization failed: %s\n", session.errbuf);

		if (ret == POISON_LIBNET_ERR)
		{
			fprintf(stderr, "Libnet error:%s\n", session.libnet_err);
		}
		return -1;
	}

	return 0;
}

void test_dhcp_discover()
{
	disc.txid = 0x01020304;
	memcpy((char *)&disc.clientmac, "\x00\x10\x22\x4b\x1c\x00", 6);

	if (poison_dhcp_discover(&session, &disc) != POISON_OK)
	{
		fprintf(stderr, "libpoison: %s\n", session.errbuf);
		fprintf(stderr, "libnet: %s\n", session.libnet_err);

		CU_FAIL("dhcp discover failed!");
	}
}

int test_dhcp_cleanup()
{
	poison_shutdown(&session);
	return 0;
}
