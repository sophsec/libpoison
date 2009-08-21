#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <libpoison.h>
#include <poison_init.h>
#include <CUnit/Basic.h>

#include "test_session.h"
#include "test_config.h"

int test_suite_session()
{
	CU_pSuite suite;

	if ((suite=CU_add_suite("session", test_session_init, test_session_cleanup)) == NULL)
	{
		return -1;
	}

	if (CU_add_test(suite, "test poison_check_init", test_session_check_init) == NULL)
	{
		return -1;
	}

	return 0;
}

static poison_session_t session;

int test_session_init()
{
	if (poison_init(&session, &test_device) != POISON_OK)
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

void test_session_check_init()
{
	if (poison_check_init(&session) != POISON_ALREADY_INIT)
	{
		CU_FAIL("check init failed\n");
	}
}

int test_session_cleanup()
{
	poison_shutdown(&session);
	return 0;
}
