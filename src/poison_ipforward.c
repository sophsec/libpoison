
/* code to enable/disable ip forwarding in the kernel */

#include "poison_route.h"
#include "libpoison.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* sets '1' in PROCIPFORWARD file */
int poison_enable_ipforward(void)
{
	int fd;
	char buf[]="1";

	/* open the file */
	fd = open(PROCIPFORWARD, O_WRONLY | O_CREAT | O_TRUNC, 
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	
	if (fd == -1)
	{
		return POISON_ERROR;
	}
	

	/* set the status */
	if (1 != write(fd, buf, 1))
	{
		close(fd);
		return POISON_ERROR;
	}

	close(fd);

	return POISON_OK;
}

/* sets '0' in PROCIPFORWARD file */
int poison_disable_ipforward(void)
{
	int fd;
	char buf[]="0";

	/* open the file */
	fd = open(PROCIPFORWARD, O_WRONLY | O_CREAT | O_TRUNC, 
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	
	if (fd == -1)
	{
		return POISON_ERROR;
	}
	
	/* set the status */
	if (1 != write(fd, buf, 1))
	{
		close(fd);
		return POISON_ERROR;
	}

	close(fd);

	return POISON_OK;
}

/* Returns IPFORWARD_ON, IPFORWARD_OFF, or POISON_ERROR */
int poison_status_ipforward(void)
{
	int fd;
	char buf[2];

	buf[0]=0;	
	buf[1]=0;

	/* open the file */
	fd = open(PROCIPFORWARD, O_RDONLY);
	
	if (fd == -1)
	{
		return POISON_ERROR;
	}
	
	/* read the data */
	if (1 != read(fd, buf, 1))
	{
		close(fd);
		return POISON_ERROR;
	}

	close(fd);

	/* check it and return accordingly */
	if (buf[0] == '1')
	{
		return IPFORWARD_ON;
	}
	
	if (buf[0] == '0')
	{
		return IPFORWARD_OFF;
	}

	/* unrecognized value */
	return POISON_ERROR;	
}
