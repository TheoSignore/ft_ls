#include "ft_ls.h"

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

void	get_dir(char* av, char options, char flag, char flag2)
{
	file_t*	file = create_new_file(av);
	if (!file)
		return;
	if (flag && !(options & LONG_LISTING))
	{
		if (stat(file->path, &(file->stats)))
		{
			access_error(file->path);
			free_file(file);
			return;
		}
	}
	else
	{
		if (lstat(file->path, &(file->stats)))
		{
			access_error(file->path);
			free_file(file);
			return;
		}
	}
	if (S_ISDIR(file->stats.st_mode) && (flag || (!flag && !(file->name[0] == '.' && (!file->name[1] || (file->name[1] == '.' && !file->name[2]))))))
	{
		get_content(file, options);
		write(1, "\n", 1);
		display_dir(file, options, flag2);
		if (options & RECURSIVE)
		{
			file_t*	tmp = file->content;
			while (tmp)
			{
				get_dir(tmp->path, options, 0, flag2);
				tmp = tmp->next;
			}
		}
	}
	else if (flag)
	{
		display_dir(file, options, flag2);
	}
	free_file(file);
}

int	sort_em(char** av)
{
	char*	tmp;

	for (size_t j = 0 ; av[j] ; j++)
	{
		for (size_t i = 0 ; av[i + 1] ; i++)
		{
			if (ft_strcmp(av[i], av[i + 1]) > 0)
			{
				tmp = av[i];
				av[i] = av[i + 1];
				av[i + 1] = tmp;
			}
		}
	}
	int	dir = 0;
	struct stat	fstat;
	while (av[dir])
	{
		lstat(av[dir], &fstat);
		if (S_ISDIR(fstat.st_mode))
			break;
		dir++;
	}
	if (!av[dir] || !av[dir + 1])
		return dir;
	int		index = dir + 1;
	while (av[index])
	{
		if (lstat(av[index], &fstat) || !(S_ISDIR(fstat.st_mode)))
		{
			tmp = av[index];
			for (int i = index; i > dir ; i--)
				av[i] = av[i - 1];
			av[dir] = tmp;
			dir++;
		}
		index++;
	}
	return dir;
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
	sort_em(av);

	char*	default_path = "."; // string literals are read-only
	if (!ac)
		av[ac++] = default_path;

	for (int i = 0 ; i < ac ; i++)
	{
		get_dir(av[i], options, 1, ac != 1);
	}

	return (0);
}
