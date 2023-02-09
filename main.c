#include "ft_ls.h"
#include <stdio.h>

void	print_bits(char c)
{
	char	bits = 8;
	char	buffer[9];
	int	i = 0;

	buffer[8] = '\n';

	while (bits > 0)
	{
		if (c & 128)
			buffer[i] = '1';
		else
			buffer[i] = '0';
		c <<= 1;
		bits--;
		i++;
	}
	write(1, buffer, 8);
}

void    access_error(char* path)
{
	char*	error_prefix = "ft_ls: cannot access '";
	char	buffer[22 + ft_strlen(path) + 2];
	size_t	i = 0;
	while (error_prefix[i])
	{
		buffer[i] = error_prefix[i];
		i++;
	}
	while (*path)
	{
		buffer[i] = *path;
		i++;
		path++;
	}
	buffer[i++] = '\'';
	buffer[i] = '\0';
	perror(buffer);
}

file_t*	get_dirs(int ac, char** av, char options, char flag)
{
	static int	uses = 0;
	for (int i = 0; i < ac ; i++)
	{
		file_t*	file = create_new_file(av[i]);
		if (!file)
			continue ;
		if (flag && !(options & LONG_LISTING))
		{
			if (stat(file->path, &(file->stats)))
			{
				access_error(file->path);
				free_file(file);
				continue ;
			}
		}
		else
		{
			if (lstat(file->path, &(file->stats)))
			{
				access_error(file->path);
				free_file(file);
				continue ;
			}
		}
		if (S_ISDIR(file->stats.st_mode) && (flag || (!flag && !(file->name[0] == '.' && (!file->name[1] || (file->name[1] == '.' && !file->name[2]))))))
		{
			get_content(file, options);
			if (uses)
				write(1, "\n", 1);
			else
				uses = 1;
			display_dir(file, options, ac);
			if (options & RECURSIVE)
			{
				file_t*	tmp = file->content;
				while (tmp)
				{
					get_dirs(1, &(tmp->path), options, 0);
					tmp = tmp->next;
				}
			}
		}
		else if (flag)
		{
			display_dir(file, options, ac);
			if (i != (ac - 1))
				write(1, "\n", 1);
		}
		free_file(file);
	}
	return (NULL);
}

int	main(int ac, char** av)
{
	char	options = get_options(ac, av);

	if (options & UNKNOWN)
	{
		char	buffer[73] = "ft_ls: invalid option '~'\nValid options are 'l', 'R', 'a', 'r', 't'\n";
		buffer[24] = options ^ UNKNOWN;
		write(2, buffer, 72);
		return (1);
	}
	ac = remove_options_and_name(ac, av);
	char*	default_path = "."; // string literals are read-only
	if (!ac)
		av[ac++] = default_path;

	//printf ("%i\n", ac);
	//for (int i = 0 ; i < ac ; i++)
	//{
	//	printf("[%i]\t\"%s\"\n", i, av[i]);
	//}

	get_dirs(ac, av, options, 1);

	return (0);
}
