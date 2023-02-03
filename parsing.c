#include "ft_ls.h"

int	remove_options_and_name(int ac, char** av)
{
	int	index = 0;

	for (int i = 1 ; i < ac ; i++)
	{
		if (av[i][0] != '-')
		{
			av[index] = av[i];
			index++;
		}
	}
	av[index] = NULL;
	return (index);
}

char	get_options(int ac, char** av)
{
	char	res = 0;
	char*	options = "lRart";
	char	option_mask[5] = { LONG_LISTING, RECURSIVE, ALL, REVERSE, SORT_MTIME };

	for (int i = 0 ; i < ac ; i++)
	{
		if (av[i][0] == '-')
		{
			char*	tmp = &(av[i][1]);
			while (*tmp)
			{
				char	b = 0;
				for (int opt_i = 0 ; opt_i < 5 ; opt_i++)
				{
					if (options[opt_i] == *tmp)
					{
						res = res | option_mask[opt_i];
						b = 1;
						break;
					}
				}
				if (!b)
				{
					res = *tmp | UNKNOWN;
					return (res);
				}
				tmp++;
			}
		}
	}
	return (res);
}
