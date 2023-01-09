#ifndef TIME_STUFF_H
# define TIME_STUFF_H
# include <stddef.h>
# include <time.h>

typedef	struct my_time_s
{
	int	year;
	int	month;
	int	day;
	int	seconds;
}	mytime_t;

void	extract_and_format_time(time_t mtim, char* str_mtim);

#endif
