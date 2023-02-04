#include "ft_ls.h"

static int	li_compare(long int n, long int save)
{
	if (save > 0)
		return (n > 0);
	return (n < 0);
}

char*	ft_litoa(long int n)
{
	size_t	res_max_size = 20;
	size_t	sbuf = res_max_size - 1;
	char	buffer[res_max_size + 1];

	for (size_t i = 0 ; i < (res_max_size + 1) ; i++)
		buffer[i] = '\0';
	buffer[sbuf] = '0';

	long int	save = n;
	long int	mfactor = n < 0 ? -10 : 10;
	long int	dfactor = 10;
	while (li_compare(n, save))
	{
		if (save > 0)
			buffer[sbuf] = (n % mfactor) + '0';
		else
			buffer[sbuf] = (n % mfactor)*(-1) + '0';
		n /= dfactor;
		sbuf--;
	}
	if (save < 0)
		buffer[sbuf] = '-';
	else if (save != 0)
		sbuf++;
	return (ft_strdup(&(buffer[sbuf])));
}

char*	ft_luitoa(long unsigned int n)
{
	size_t	res_max_size = 20;
	size_t	sbuf = res_max_size - 1;
	char	buffer[res_max_size + 1];

	for (size_t i = 0 ; i < (res_max_size + 1) ; i++)
		buffer[i] = '\0';
	buffer[sbuf] = '0';

	long int	factor = 10;
	long int	save = n;
	while (n > 0)
	{
		buffer[sbuf] = (n % factor) + '0';
		n /= factor;
		sbuf--;
	}
	if (save != 0)
		sbuf++;
	return (ft_strdup(&(buffer[sbuf])));
}

char*	ft_uitoa(unsigned int n)
{
	size_t	res_max_size = 10;
	size_t	sbuf = res_max_size - 1;
	char	buffer[res_max_size + 1];

	for (size_t i = 0 ; i < (res_max_size + 1) ; i++)
		buffer[i] = '\0';
	buffer[sbuf] = '0';

	unsigned int	save = n;
	unsigned int	factor = 10;
	while (n > 0)
	{
		buffer[sbuf] = (n % factor) + '0';
		n /= factor;
		sbuf--;
	}
	if (save != 0)
		sbuf++;
	return (ft_strdup(&(buffer[sbuf])));
}
