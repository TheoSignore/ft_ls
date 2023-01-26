#include "ft_ls.h"
#include <stdio.h>

void	print_bits(char c)
{
	char	bits = 8;
	char	buffer[9];
	int	i = 0;

	buffer[8] = '\0';

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
	printf("%s\n", buffer);
}

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

size_t	ft_strlen(const char* str)
{
	size_t	res = 0;

	if (str)
		while (str[res])
			res++;
	return (res);
}

char*	ft_strdup(const char* str)
{
	if (!str)
		return (NULL);
	size_t	str_len = ft_strlen(str);
	char*	res = malloc(str_len + 1);
	if (res)
	{
		for (size_t i = 0 ; i < str_len ; i++)
			res[i] = str[i];
		res[str_len] = '\0';
	}
	return (res);
}

static file_t*	create_new_file(char* file_path)
{
	file_t*	res = malloc(sizeof(file_t));
	if (res)
	{
		res->path = ft_strdup(file_path);
		size_t	i = 0;
		while (res->path[i])
			i++;
		i--;
		if (res->path[i] == '/')
			i--;
		while (i > 0 && res->path[i] != '/')
			i--;
		if (i)
			i++;
		res->name = &(res->path[i]);

		res->stats.st_nlink = 0;
		res->content = NULL;
		res->info = NULL;
		res->next = NULL;
	}
	else
		perror("ft_ls:");
	return res;
}


static void	access_error(char* path)
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

static void	add_back_file(file_t** list, file_t* el)
{
	file_t*	tmp = *list;
	if (!tmp)
	{
		*list = el;
		return ;
	}
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = el;
}


static void	free_file(file_t* fp)
{
	if (fp)
	{
		free(fp->path);
		free_file(fp->content);
		free(fp->info);
	}
}

static void	free_file_chain(file_t* fpc)
{
	if (!fpc)
		return ;
	file_t*	tmp = fpc->next;
	free_file(fpc);
	free_file_chain(tmp);
}

static char*	join_path(char* str1, char* str2)
{
	size_t	len1 = ft_strlen(str1);
	size_t	len2 = ft_strlen(str2);
	size_t	len_res = len1 + len2 + (str1[len1 - 1] == '/' ? 1 : 2);
	char*	res = malloc(len_res);
	if (!res)
		return (NULL);
	for (size_t i = 0 ; i < len1 ; i++)
	{
		*res = str1[i];
		res++;
	}
	if (str1[len1 - 1] != '/')
	{
		*res = '/';
		res++;
	}
	for (size_t i = 0 ; i <= len2 ; i++)
	{
		*res = str2[i];
		res++;
	}
	res -= len_res;
	return (res);
}

static void	get_content(file_t* dir, char options)
{
	DIR*	dir_stream = opendir(dir->path);
	if (!dir_stream)
	{
		return ;
	}
	errno = 0;
	struct dirent*	subfile = readdir(dir_stream);
	while (subfile)
	{
		if (!(subfile->d_name[0] == '.' && !(options & ALL)))
		{
			char*	subfile_path = join_path(dir->path, subfile->d_name);
			file_t*	file = create_new_file(subfile_path);
			free(subfile_path);
			if (!file)
			{
				break ;
			}
			//	if (options & RECURSIVE)
			//	{
			//	}
			add_back_file(&(dir->content), file);
		}
		errno = 0;
		subfile = readdir(dir_stream);
	}
	closedir(dir_stream);
	if (errno != 0)
		free_file(dir->content);
}

file_t*	get_dirs(int ac, char** av, char options)
{
	(void)options;
	file_t*	res = NULL;

	for (int i = 0; i < ac ; i++)
	{
		file_t*	file = create_new_file(av[i]);
		if (!file)
			free_file_chain(res);
		if (stat(file->path, &(file->stats)))
		{
			access_error(file->path);
			free_file(file);
			continue ;
		}
		if (S_ISDIR(file->stats.st_mode))
		{
			get_content(file, options);
		}
		add_back_file(&res, file);
	}
	return (res);
}

static void	display_list(file_t* file, char options)
{
	(void)options;
	if (file)
	{
		if (file->content)
			printf("%s:\n", file->path);
		else
			printf("%s ", file->name);
		if (file->content)
		{
			display_list(file->content, options);
			printf("\n\n");
		}
		display_list(file->next, options);
	}
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
	else
		write(1, (options ? "Options used: -" : "No option used."), 15);
	if (options & LONG_LISTING)
		write(1, "l", 1);
	if (options & RECURSIVE)
		write(1, "R", 1);
	if (options & ALL)
		write(1, "a", 1);
	if (options & REVERSE)
		write(1, "r", 1);
	if (options & SORT_MTIME)
		write(1, "t", 1);
	write(1, "\n", 1);

	ac = remove_options_and_name(ac, av);
	char*	default_path = "."; // string literals are read-only
	if (!ac)
		av[ac++] = default_path;

	printf ("%i\n", ac);
	for (int i = 0 ; i < ac ; i++)
	{
		printf("[%i]\t\"%s\"\n", i, av[i]);
	}


	file_t*	list = get_dirs(ac, av, options); // handles the -a
	
	
	//if (options & RECURSIVE)
	//	expand_dirs(list, options & ALL); // handles -R and -a in subdirectories
	//sort_listing(&list, (options & REVERSE), (options & SORT_MTIME)); // handles default sorting, -r and -t
	display_list(list, options); // handles -l
	free_file_chain(list);
	return (0);
}
//11 + 1 + max_len(number_of_hard_links) + 1 + max_len(uname) + 1 + max_len(gname) + 1 + max_len(size) + 1 + 12 + 1 + max_len(file_name);
