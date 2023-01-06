#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

typedef	struct my_time_s
{
	int	year;
	int	month;
	int	day;
	int	seconds;
}	mytime_t;

static int	year_atoi(char* year)
{
	int res = year[3] - 48;
	res += (year[2] - 48) * 10;
	res += (year[1] - 48) * 100;
	res += (year[0] - 48) * 1000;
	return (res);
}

static int	month_to_i(char* month)
{
	if (month[0] == 'F')
		return (2);
	if (month[0] == 'S')
		return (9);
	if (month[0] == 'O')
		return (10);
	if (month[0] == 'N')
		return (11);
	if (month[0] == 'D')
		return (12);
	if (month[0] == 'M')
	{
		if (month[2] == 'r')
			return (3);
		return (5);
	}
	if (month[1] == 'a')
		return (1);
	if (month[3] == 'n')
		return (6);
	return (7);
}

static int	day_atoi(char* day)
{
	return	((day[1] - 48) + ((day[0] == ' ') ? 0 : (day[0] - 48) * 10));
}

// "hour:minute:seconds" to seconds
static size_t	ahms_to_isec(char* hms)
{
	size_t res = (hms[7] - 48) + ((hms[6] - 48) * 10);
	res += ((hms[4] - 48) + ((hms[3] - 48) * 10)) * 60;
	res += ((hms[1] - 48) + ((hms[0] - 48) * 10)) * 3600;
	return (res);
}

//Wed Jun 30 21:49:08 1993\n
//0   4   8  11       20
static void	ctime_to_my_time(char* sctime, mytime_t* mytime)
{
	mytime->year = year_atoi(&sctime[20]);
	mytime->month = month_to_i(&sctime[4]);
	mytime->day = day_atoi(&sctime[8]);
	mytime->seconds = ahms_to_isec(&sctime[11]);
}

static void	print_mytime(mytime_t* mytime)
{
	printf(
		"%i years, %i months, %i days and %i seconds.\n",
		mytime->year,
		mytime->month,
		mytime->day,
		mytime->seconds
	);
}

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

static char	is_recent(time_t timeval, char* str_timeval)
{
	static time_t	limit = 0;
	static time_t	current_time = 0;
	static mytime_t	current_mytime;

	if (limit != 0 && timeval <= limit)
		return (0);
	if (current_time == 0)
	{
		current_time = time(NULL);
		ctime_to_my_time(ctime(&current_time), &current_mytime);
	}
	mytime_t	mytime_file;
	ctime_to_my_time(str_timeval, &mytime_file);

	if ((current_mytime.year - mytime_file.year) >= 2)
		return (0);
	int	diff = current_mytime.month - mytime_file.month;
	diff = diff < 0 ? diff + 12 : diff;
	if (diff < 6)
		return (1);
	if (diff > 6)
		return (0);
	if (mytime_file.day > current_mytime.day)
		return (1);
	if (mytime_file.day < current_mytime.day)
		return (0);
	return (mytime_file.seconds >= current_mytime.seconds);
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
	mytime_t	current_mytime;
	mytime_t	file_mytime;

	ctime_to_my_time(ctime(&current_time), &current_mytime);
	printf("current time: '%s'\n", ctime(&current_time));
	print_mytime(&current_mytime);

	ctime_to_my_time(ctime(&stats.st_mtim.tv_sec), &file_mytime);
	printf("file time: '%s'\n", ctime(&stats.st_mtim.tv_sec));
	print_mytime(&file_mytime);
	return (0);
}

//11 + 1 + max_len(number_of_hard_links) + 1 + max_len(uname) + 1 + max_len(gname) + 1 + max_len(size) + 1 + 12 + 1 + max_len(file_name);

