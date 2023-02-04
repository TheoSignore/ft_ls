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

//static void	print_lldir(file_t* dir)
//{
//	allocate and fill content "info" section
//	get biggest lens of of them
//	print content
//}

//11 + 1 + max_len(number_of_hard_links) + 1 + max_len(uname) + 1 + max_len(gname) + 1 + max_len(size) + 1 + 12 + 1 + max_len(file_name);


void	fill_byte(void* ptr, size_t n, char c)
{
	char*	cptr = ptr;
	for (size_t i = 0 ; i < n ; i++)
		cptr[i] = c;
}

void	write_info_to_buffer(char* buff, file_t* file, lens_t* lens)
{
	size_t	i = 0;
	fileinfo_t*	info = file->info;
	buff[i] = info->file_type;
	i++;
	ft_memcpy(&(buff[i]), info->perms, 9);
	i += 9 + 1;
	i += lens->nlinks - info->lens.nlinks;
	ft_memcpy(&(buff[i]), info->number_of_links, info->lens.nlinks);
	i += info->lens.nlinks + 1;
	ft_memcpy(&(buff[i]), info->owner_name, info->lens.oname);
	i += info->lens.oname + 1;
	ft_memcpy(&(buff[i]), info->group_name, info->lens.gname);
	i += info->lens.oname + 1;
	size_t	i_save = i;
	if (buff[0] == 'c' || buff[0] == 'b')
	{
		i += lens->maj_min_siz - info->lens.major - 2 - lens->minor;
		ft_memcpy(&(buff[i]), info->major, info->lens.major);
		i += info->lens.major;
		buff[i] = ',';
		i = i_save;
		i += lens->maj_min_siz - info->lens.minor;
		ft_memcpy(&(buff[i]), info->minor, info->lens.minor);
	}
	else
	{
		i += lens->maj_min_siz - info->lens.size;
		ft_memcpy(&(buff[i]), info->size, info->lens.size);
	}
	i = i_save;
	i += lens->maj_min_siz + 1;
	ft_memcpy(&(buff[i]), info->mtime, 12);
	i += 13;
	size_t	filename_len = ft_strlen(file->name);
	ft_memcpy(&(buff[i]), file->name, filename_len);
	i += filename_len;
	if (info->target)
	{
		buff[i++] = '-';
		buff[i++] = '>';
		i++;
		ft_memcpy(&(buff[i]), info->target, ft_strlen(info->target));
		i += ft_strlen(info->target);
	}
	buff[i] = '\n';
}

static void	print_llfile(file_t* file, lens_t* lens)
{
	if (!file->info)
		load_file_info(file);
	if (!lens)
		lens = &(file->info->lens);
	if (file->info->file_type == 'c' || file->info->file_type == 'b')
		printf("%c%s %s %s %s %s, %s %s %s\n",
				file->info->file_type,
				file->info->perms,
				file->info->number_of_links,
				file->info->owner_name,
				file->info->group_name,
				file->info->major,
				file->info->minor,
				file->info->mtime,
				file->name);
	else if (file->info->file_type == 'l')
		printf("%c%s %s %s %s %s %s %s -> %s\n",
				file->info->file_type,
				file->info->perms,
				file->info->number_of_links,
				file->info->owner_name,
				file->info->group_name,
				file->info->size,
				file->info->mtime,
				file->name,
				file->info->target);
	else
	{
		size_t	bufflen = 29 + lens->nlinks + lens->oname + lens->gname + lens->maj_min_siz + ft_strlen(file->name);
		char*	buffer = malloc(bufflen);
		fill_byte(buffer, bufflen - 1, ' ');
		write_info_to_buffer(buffer, file, lens);
		write(1, buffer, bufflen);
		free(buffer);
	}
}


//trwxrwxrwx <hardlinks> <uname> <gname> <<major>, <minor>|<size>> XXX_XX_XXXXX <name>[ -> target]\n
//|_________|           |       |       |                         |____________|                  |
// 11                   1       1       1                            14                           1
// 11 + 1 + 1 + 1 + 14 + 1 = 29
// 29 + hl_len + uname_len + gname + (major_len + 2 + major _len|size_len) + filename_len [+ 2 + target_len]

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
			else
				print_llfile(file, NULL);
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
