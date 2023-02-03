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

static char*	get_link_target(char* path)
{
	size_t	sbuf = 16;
	size_t	target_len = 16;
	while (1)
	{
		char	buffer[sbuf];
		target_len = readlink(path, buffer, sbuf);
		if (target_len == (size_t)(-1))
			return (NULL);
		else if (target_len < sbuf)
		{
			buffer[target_len] = '\0';
			return (ft_strdup(buffer));
		}
		sbuf *= 2;
	}
	return (NULL);
}

char*	ft_itoa(int n)
{
	size_t	sbuf = 10;
	char	buffer[12];

	for (int i = 0 ; i < 12 ; i++)
		buffer[i] = '\0';

	char	sign = n < 0 ? 0 : 1;
	long int	li = n < 0 ? -n : n;
	while (li > 0)
	{
		buffer[sbuf] = (li % 10) + '0';
		li /= 10;
		sbuf--;
	}
	if (!sign)
		buffer[sbuf] = '-';
	else
		sbuf++;
	return (ft_strdup(&(buffer[sbuf])));
}

static char*	get_owner(int uid)
{
	struct passwd*	spwd = getpwuid(uid);
	if (spwd)
		return (ft_strdup(spwd->pw_name));
	return (ft_itoa(uid));
}

static char*	get_group(int gid)
{
	struct group*	grp = getgrgid(gid);
	if (grp)
		return (ft_strdup(grp->gr_name));
	return (ft_itoa(gid));
}

static void	load_file_info(file_t* file)
{
	if (file->stats.st_nlink == 0)
	{
		if (lstat(file->path, &(file->stats)))
			return;
	}
	fileinfo_t*	info = malloc(sizeof(fileinfo_t));
	if (!info)
		return ;
	info->file_type = get_type(file->stats.st_mode);
	get_perms(file->stats.st_mode, info->perms);
	info->number_of_links = file->stats.st_nlink;
	info->owner_name = get_owner(file->stats.st_uid);
	info->group_name = get_group(file->stats.st_gid);
	info->minor = ft_itoa(minor(file->stats.st_dev));
	info->major = ft_itoa(major(file->stats.st_dev));
	info->size = ft_itoa(file->stats.st_size);
	info->blksize = ft_itoa(file->stats.st_blksize);
	extract_and_format_time(file->stats.st_mtime, info->mtime);
	if (info->file_type == 'l')
		info->target = get_link_target(file->path);
	else
		info->target = NULL;
	file->info = info;
}


static void	print_llfile(file_t* file, lens_t* lens)
{
	(void)lens;
	if (!file->info)
		load_file_info(file);
	printf("%c%s %zu %s %s %s %s %s %s %s\n",
			file->info->file_type,
			file->info->perms,
			file->info->number_of_links,
			file->info->owner_name,
			file->info->group_name,
			file->info->minor,
			file->info->major,
			file->info->size,
			file->info->blksize,
			file->info->mtime);
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
