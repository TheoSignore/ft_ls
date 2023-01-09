#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <pwd.h>
#include <grp.h>
#include "time_stuff.h"

size_t	ft_strlen(char const *s)
{
	size_t	size = 0;
	if (s)
	{
		while (s[size])
			size++;
	}
	return (size);
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

//Wed Jun 30 21:49:08 1993\n
//0   4   8  11       20
//    0   4  7        16
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
		"%s %lu %s %s",
		buf,
		stats.st_nlink,
		(getpwuid(stats.st_uid)->pw_name),
		(getgrgid(stats.st_gid)->gr_name)
	);
	if (stats.st_rdev)
		printf(" %u, %u", major(stats.st_rdev), minor(stats.st_rdev));
	else
		printf(" %li", stats.st_size);
	char	mtime[13];
	extract_and_format_time(stats.st_mtime, mtime);
	printf(
		" %s %s\n",
		mtime,
		av[1]
	);
	return (0);
}

//11 + 1 + max_len(number_of_hard_links) + 1 + max_len(uname) + 1 + max_len(gname) + 1 + max_len(size) + 1 + 12 + 1 + max_len(file_name);
