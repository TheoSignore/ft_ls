#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/xattr.h>

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
	write(1, " ", 1);
}

static char	get_type(mode_t mode)
{
	if (S_ISREG(mode))
		return '-';
	else if (S_ISDIR(mode))
		return 'd';
	else if (S_ISLNK(mode))
		return 'l';
	else if (S_ISCHR(mode))
		return 'c';
	else if (S_ISBLK(mode))
		return 'b';
	else if (S_ISFIFO(mode))
		return 'f';
	else if (S_ISSOCK(mode))
		return 's';
	return '?';
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
	printf("\n%s %li %lu %lu %u %u %lu %li %li %li %s\n",
		buf,
		stats.st_dev,
		stats.st_ino,
		stats.st_nlink,
		stats.st_uid,
		stats.st_gid,
		stats.st_rdev,
		stats.st_size,
		stats.st_blksize,
		stats.st_blocks,
		av[1]
	);
	return (0);
}
