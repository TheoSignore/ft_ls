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



static void	print_content(file_t* files)
{
	size_t	buffer_len = 0;
	file_t*	tmp = files;
	while (tmp)
	{
		buffer_len += ft_strlen(tmp->name) + (buffer_len ? 2 : 0);
		tmp = tmp->next;
		if (buffer_len >= BUFFER_SIZE || !tmp)
		{
			buffer_len++;
			char	buffer[buffer_len];
			size_t	i = 0;
			size_t	len = 0;
			while (files != tmp)
			{
				len = ft_strlen(files->name);
				ft_memcpy(&buffer[i], files->name, len);
				i += len;
				buffer[i++] = ' ';
				buffer[i++] = ' ';
				files = files->next;
			}
			buffer[buffer_len - 1] = '\n';
			write(1, buffer, buffer_len);
			buffer_len = 0;
		}
	}
}

static void	display_dir(file_t* file, char long_listing, int flag)
{
	(void)long_listing;
	if (file)
	{
		if (file->content)
		{
			if (flag != 1)
			{
				size_t	len = ft_strlen(file->path);
				char	buffer[len + 2];
				buffer[len] = ':';
				buffer[len + 1] = '\n';
				ft_memcpy(buffer, file->path, len);
				write(1, buffer, len + 2);
			}
			if (!long_listing)
				print_content(file->content);
//			else
//				print_lldir(file);
		}
		else
		{
			if (!long_listing)
			{
				size_t	len = ft_strlen(file->name);
				char	buffer[len + 1];
				buffer[len] = '\n';
				ft_memcpy(buffer, file->name, len);
				write(1, buffer, len + 1);
			}
//			else
//				print_llfile(file);
		}
	}
}


file_t*	get_dirs(int ac, char** av, char options, char flag)
{
	for (int i = 0; i < ac ; i++)
	{
		file_t*	file = create_new_file(av[i]);
		if (!file)
			continue ;
		if (flag)
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
			display_dir(file, options & LONG_LISTING, ac);
			if (i != (ac - 1))
				write(1, "\n", 1);
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
			display_dir(file, options & LONG_LISTING, ac);
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
//11 + 1 + max_len(number_of_hard_links) + 1 + max_len(uname) + 1 + max_len(gname) + 1 + max_len(size) + 1 + 12 + 1 + max_len(file_name);
