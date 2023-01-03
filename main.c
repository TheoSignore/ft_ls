#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <sys/acl.h>
size_t printed = 0;
size_t foo = 0;
void	print_bits(char c)
{
	char	bits;

	bits = 8;
	while (bits > 0)
	{
		if (c & 128)
			write(1, "1", 1);
		else
			write(1, "0", 1);
		c <<= 1;
		bits--;
	}
	printed++;
	write(1, " ", 1);
	foo++;
	if ( foo >= 4)
	{
		write(1, "\b\n", 2);
		foo = 0;
	}
}

int	main(int ac, char** av)
{
	acl_t	base;
	(void)base;
	printf("%zu\n", sizeof(acl_t));
	if (ac != 2)
		return (1);
	struct stat	stats;
	if (lstat(av[1], &stats))
	{
		perror("FAILED");
		return (1);
	}
	char	s[100];
	size_t	a = listxattr(av[1], s, 100);
	write(1, s, a);
	write(1, "\n", 1);
	a = getxattr(av[1], "system.nfs4_acl", s, 100);
	//write(1, s, a);
	char flag = 0;
	for (size_t i = 0; i < 80 ; i++)
	{
		if (s[i] >= 'A' && s[i] <= 'Z')
		{
			foo = 0;
			if (!flag)
			{
				flag = 1;
				write(1, "\n", 1);
			}
			write(1, &(s[i]), 1);
			printed++;
		}
		else
		{
			if (flag)
			{
				flag = 0;
				write(1, "\n", 1);
			}
			print_bits(s[i]);
		}
	}
	printf("\n%zu\n%li %lu %u %lu %u %u %lu %li %li %li %s %s\n",
		printed,
		stats.st_dev,
		stats.st_ino,
		stats.st_mode,
		stats.st_nlink,
		stats.st_uid,
		stats.st_gid,
		stats.st_rdev,
		stats.st_size,
		stats.st_blksize,
		stats.st_blocks,
		av[1],
		s
	);
	printf("%i\n", *((int*)(&(s[76]))));
	return (0);
}
