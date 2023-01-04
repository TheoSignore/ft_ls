#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <sys/sysmacros.h>

char lel = 0;
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
	write (1, " ", 1);
	lel++;
	if (lel >= 4)
	{
		write(1, "\n", 1);
		lel = 0;
	}
}

static char	get_type(mode_t mode)
{
	switch (mode & S_IFMT)
	{
		case S_IFREG:
			return '-';
		case S_IFDIR:
			return 'd';
		case S_IFLNK:
			return 'l';
		case S_IFSOCK:
			return 's';
		case S_IFBLK:
			return 'b';
		case S_IFCHR:
			return 'c';
		case S_IFIFO:
			return 'p';
		default:
			return '?';
	}
}

static void	get_perms(mode_t mode, char* buf)
{
	buf[0] = (mode & S_IRUSR) ? 'r' : '-';
	buf[1] = (mode & S_IWUSR) ? 'w' : '-';
	buf[2] = (mode & S_IXUSR) ? 'x' : '-';
	buf[3] = (mode & S_IRGRP) ? 'r' : '-';
	buf[4] = (mode & S_IWGRP) ? 'w' : '-';
	buf[5] = (mode & S_IXGRP) ? 'x' : '-';
	buf[6] = (mode & S_IROTH) ? 'r' : '-';
	buf[7] = (mode & S_IWOTH) ? 'w' : '-';
	buf[8] = (mode & S_IXOTH) ? 'x' : '-';
}

int	main(int ac, char** av)
{
	if (ac != 2)
		return (1);
	struct stat	stats;
	if (lstat(av[1], &stats))
	{
		perror("FAILED");
		return (1);
	}
	char	buf[11];
	buf[10] = '\0';
	buf[0] = get_type(stats.st_mode);
	get_perms(stats.st_mode, &(buf[1]));
	printf(
		"%s hl=%lu oid=%u gid=%u",
		buf,
		stats.st_nlink,
		stats.st_uid,
		stats.st_gid
	);
	if (stats.st_dev)
		printf(" maj=%u min=%u", major(stats.st_rdev), minor(stats.st_rdev));
	else
		printf(" 0");
	printf(
		" sz=%li bsz=%li blk=%li %s\n",
		stats.st_size,
		stats.st_blksize,
		stats.st_blocks,
		av[1]
	);
	char	xattr[1000];
	printf (" (%zu)\n", write(1, xattr, listxattr(av[1], xattr, 1000)));
	char	attr_value[1000];
	size_t	fock = getxattr(av[1], xattr, attr_value, 1000);
	printf(" (%zu)\n", write(1, attr_value, fock));
	char flag = 0;
	for (size_t i = 0 ; i < fock ; i++)
	{
		if (attr_value[i] > ' ' && attr_value[i] <= '~')
		{
			lel = 0;
			if (!flag)
			{
				flag = 1;
				printf("\n");
			}
			printf("%c", attr_value[i]);
		}
		else
		{
			if (flag)
			{
				flag = 0;
				printf("\n");
			}
			printf("%x ", attr_value[i]);
			//print_bits(attr_value[i]);
		}
	}
	return (0);
}
