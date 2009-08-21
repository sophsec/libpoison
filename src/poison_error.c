#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "libpoison.h"
#include "poison_error.h"


char *poison_perror(int error)
{
	int loop;

	poison_error_table_t table[]=
	{
		{POISON_OK, "No error"},
		{POISON_ERR, "Generic Error"},
		{POISON_SESSION_NULL, "The value passed for the poison_session_t was NULL"},
		{POISON_INITIALIZED, "LibPoison session has been initialized"},
		{POISON_NOT_INITIALIZED, "Session has not been initialized"},
		{POISON_LIBNET_ERR, "Libnet has returned back an error"},
		{POISON_NOMOM, "ENOMEM: Not enough memory"},
		{POISON_CORRUPT, "Poison session has been corrupted; abort"}
	}

	for (loop = 0; loop < sizeof(table)/sizeof(table[0]); ++loop)
	{
		if (table[loop].num == error)
		{
			return table[loop].str;
		}
	}

	return NULL;
}

