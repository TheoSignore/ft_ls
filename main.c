#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

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

static char	is_old_enough(time_t mtime, time_t current_time)
{
	char	str_mtime[21];
	char	str_current_time[21];

	str_mtime[20] = '\0';
	str_current_time[20] = '\0';

	char*	str_time = ctime(&mtime);
	for (size_t i = 4 ; i < 24 ; i++)
		str_mtime[i - 4] = str_time[i];
	str_time = ctime(&current_time);
	for (size_t i = 4 ; i < 24 ; i++)
		str_current_time[i - 4] = str_time[i];
	if (!ft_strcmp(&str_time[16], &str_current_time[16]))
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
		"%s %lu %s %s",
		buf,
		stats.st_nlink,
		(getpwuid(stats.st_uid)->pw_name),
		(getgrgid(stats.st_gid)->gr_name)
	);
	if (stats.st_rdev)
		printf(" %u, %u", major(stats.st_rdev), minor(stats.st_rdev));
	else
		printf(" 0 %li", stats.st_size);
	char*	date = ctime(&stats.st_mtim.tv_sec);
	//date[ft_strlen(date) - 9] = '\0';
	printf(
		" %s %s\n",
		date,
		av[1]
	);
	time_t	current_time;
	time(&current_time);
	printf("%s %zu\n", ctime(&stats.st_mtim.tv_sec), ft_strlen(date));
	return (0);
}

//11 + 1 + max_len(number_of_hard_links) + 1 + max_len(uname) + 1 + max_len(gname) + 1 + max_len(size) + 1 + 12 + 1 + max_len(file_name);
