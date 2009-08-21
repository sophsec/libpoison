#include <CUnit/Basic.h>

#include "test_session.h"
#include "test_dhcp.h"

int main()
{
	// initialize the CUnit test registry
	if (CU_initialize_registry() != CUE_SUCCESS)
	{
		goto get_errors;
	}

	// register the session suite
	if (test_suite_session() != 0)
	{
		goto cleanup;
	}

	// register the dhcp suite
	if (test_suite_dhcp() != 0)
	{
		goto cleanup;
	}

	// Run all tests using the CUnit Basic interface
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	goto get_errors;

cleanup:
	CU_cleanup_registry();
get_errors:
	return CU_get_error();
}
