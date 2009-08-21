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

	return 0;
}

static poison_session_t session;

int test_dhcp_init()
{
	if (poison_init(&session, &test_device) != POISON_OK)
	{
		fprintf(stderr, "initialization failed: %s\n", session.errbuf);
		return -1;
	}

	return 0;
}

int test_dhcp_cleanup()
{
	poison_shutdown(&session);
	return 0;
}
