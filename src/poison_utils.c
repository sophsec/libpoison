#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "poison_utils.h"

/* convert 4 hex (8 ASCII) bytes into an IP */
ip_addr_t hex2ip(char *s)
{
	ip_addr_t ip = 0;

	ip = hex2byte(s);
	ip <<= 8; 
	
	ip |= hex2byte(s+2);
	ip <<= 8;

	ip |= hex2byte(s+4);
	ip <<= 8;
	
	ip |= hex2byte(s+6);
	ip <<= 8;

	return ip;
}

/* 
	This function returns the binary
	for the first 2 printable hexbytes in s
	
	on fail binary 0 is returned
*/	
unsigned char hex2byte(char *s)
{
	char byte = 0;	
	char cur;

	cur = toupper(*s);
	
	if (cur <= 'F' && cur >= 'A') 
	{
		byte = cur - 0x37;
	}
	else if (cur <= '9' && cur >= '0')
	{
		byte = cur - 0x30;
	}
	else
	{
		return byte;
	}

	cur = toupper(*(s+1));

	if (cur <= 'F' && cur >= 'A') 
	{
		byte <<= 4;
		byte |= cur - 0x37;
	}
	else if (cur <= '9' && cur >= '0')
	{
		byte <<= 4;
		byte |= cur - 0x30;
	}

	return byte;
}

