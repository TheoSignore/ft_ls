#include "ft_ls.h"
#include "time_stuff.h"

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

//Jun 30 21:49:08 1993\n
//0   4  7        16
static void	ctime_to_my_time(char* sctime, mytime_t* mytime)
{
	mytime->year = year_atoi(&sctime[16]);
	mytime->month = month_to_i(&sctime[0]);
	mytime->day = day_atoi(&sctime[4]);
	mytime->seconds = ahms_to_isec(&sctime[7]);
}

//	static void	print_mytime(mytime_t* mytime)
//	{
//		printf(
//			"%i years, %i months, %i days and %i seconds.\n",
//			mytime->year,
//			mytime->month,
//			mytime->day,
//			mytime->seconds
//		);
//	}

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
		char*	str_tmp_current_time = ctime(&current_time);
		ctime_to_my_time(&str_tmp_current_time[4], &current_mytime);
	}
	mytime_t	mytime_file;
	ctime_to_my_time(str_timeval, &mytime_file);

	if ((current_mytime.year - mytime_file.year) >= 2)
	{
		limit = timeval;
		return (0);
	}
	int	diff = current_mytime.month - mytime_file.month;
	diff = diff < 0 ? diff + 12 : diff;
	if (diff < 6)
		return (1);
	if (diff > 6)
	{
		limit = timeval;
		return (0);
	}
	if (mytime_file.day > current_mytime.day)
		return (1);
	if (mytime_file.day < current_mytime.day)
	{
		limit = timeval;
		return (0);
	}
	if (mytime_file.seconds >= current_mytime.seconds)
		return (1);
	limit = timeval;
	return (0);
}

//Wed Jun 30 21:49:08 1993\n
//0   4   8  11       20

//Jun 30 21:49:08 1993\n
//0   4  7        16

//str_mtim -> char[13];
void	extract_and_format_time(time_t mtim, char* str_mtim)
{
	
	char*	date_tmp = ctime(&mtim);
	char	date[21];
	for (int i = 0 ; i < 21 ; i++)
		date[i] = date_tmp[i + 4];
	for (int i = 0 ; i < 13 ; i++)
		str_mtim[i] = date[i];
	if (!is_recent(mtim, date))
	{
		str_mtim[7] = ' ';
		str_mtim[8] = date[16];
		str_mtim[9] = date[17];
		str_mtim[10] = date[18];
		str_mtim[11] = date[19];
	}
	str_mtim[12] = '\0';
}
