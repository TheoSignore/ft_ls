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


//11 + 1 + max_len(number_of_hard_links) + 1 + max_len(uname) + 1 + max_len(gname) + 1 + max_len(size) + 1 + 12 + 1 + max_len(file_name);


//duck

void	write_and_inc_buffer(char** buff, char* src, size_t n)
{
	for (size_t i = 0; i < n ; i++)
	{
		**buff = src[i];
		(*buff)++;
	}
}
void	reverse_memcpy(void* dst, void* src, size_t n)
{
	char*	cdst = dst;
	char*	csrc = src;
	n--;
	while (1)
	{
		*cdst = csrc[n];
		cdst--;
		if (n == 0)
			break;
		n--;
	}
}

typedef struct ls_buffer
{
	char*	buffer;
	char*	type;
	char*	perms;
	char*	nlink;
	char*	owner;
	char*	group;
	char*	major;
	char*	minor;
	char*	size;
	char*	mtime;
	char*	name;
} lsbf_t;


void	init_lsbf(lsbf_t* lsbf, char* bf, lens_t* lens)
{
	lsbf->buffer = bf;
	lsbf->type = bf;
	lsbf->perms = &(bf[1]);
	lsbf->nlink = &(bf[10 + lens->nlinks]);
	lsbf->owner = lsbf->nlink + 2;
	lsbf->group = lsbf->owner + lens->oname + 1;
	lsbf->major = lsbf->group + lens->gname + lens->major;
	lsbf->minor = lsbf->group + lens->gname + lens->maj_min_siz;
	lsbf->size = lsbf->group + lens->gname + lens->maj_min_siz;
	lsbf->mtime = lsbf->size + 2;
	lsbf->name = lsbf->mtime + 13;
}

void	write_info_to_buffer(char* buff, file_t* file, lens_t* lens)
{
	fileinfo_t*	info = file->info;
	lsbf_t	lsbf;
	init_lsbf(&lsbf, buff, lens);
	*(lsbf.type) = info->file_type;
	ft_memcpy(lsbf.perms, info->perms, 9);
	reverse_memcpy(lsbf.nlink, info->number_of_links, info->lens.nlinks);
	ft_memcpy(lsbf.owner, info->owner_name, info->lens.oname);
	ft_memcpy(lsbf.group, info->group_name, info->lens.gname);
	if (info->file_type == 'c' || info->file_type == 'b')
	{
		reverse_memcpy(lsbf.minor, info->minor, info->lens.minor);
		reverse_memcpy(lsbf.major, info->major, info->lens.major);
		*(lsbf.major + 1) = ',';
	}
	else
	{
		reverse_memcpy(lsbf.size, info->size, info->lens.size);
	}
	ft_memcpy(lsbf.mtime, info->mtime, 12);
	size_t	filename_len = ft_strlen(file->name);
	ft_memcpy(lsbf.name, file->name, filename_len);
	if (info->target)
	{
		ft_memcpy((lsbf.name + filename_len), " -> ", 4);
		filename_len += 4;
		size_t	target_len = ft_strlen(info->target);
		ft_memcpy((lsbf.name + filename_len), info->target, target_len);
		*(lsbf.name + filename_len + target_len) = '\n';
	}
	else
		*(lsbf.name + filename_len) = '\n';
}

static void	print_llfile(file_t* file, lens_t* lens)
{
	if (!file->info)
		load_file_info(file);
	if (!file->info)
	{
		printf("can't access file\n");
		return ;
	}
	if (!lens)
		lens = &(file->info->lens);
	size_t bufflen = 0;
	char*	buffer = NULL;
	if (file->info->file_type == 'c' || file->info->file_type == 'b')
	{
		bufflen = 29 + lens->nlinks + lens->oname + lens->gname + lens->maj_min_siz + ft_strlen(file->name);
	}
	else if (file->info->file_type == 'l')
	{
		bufflen = 33 + lens->nlinks + lens->oname + lens->gname + lens->maj_min_siz + ft_strlen(file->name) + ft_strlen(file->info->target);
	}
	else
	{
		bufflen = 29 + lens->nlinks + lens->oname + lens->gname + lens->maj_min_siz + ft_strlen(file->name);
	}
	buffer = malloc(bufflen + 1);
	fill_byte(buffer, bufflen - 1, ' ');
	write_info_to_buffer(buffer, file, lens);
	write(1, buffer, bufflen);
	free(buffer);
}

static void	update_lens(lens_t* lens, lens_t* file_lens, char type)
{
	if (lens->nlinks < file_lens->nlinks)
		lens->nlinks = file_lens->nlinks;
	if (lens->oname < file_lens->oname)
		lens->oname = file_lens->oname;
	if (lens->gname < file_lens->gname)
		lens->gname = file_lens->gname;
	if (type == 'c' || type == 'b')
	{
		if (lens->major < file_lens->major)
			lens->major = file_lens->major;
		if (lens->minor < file_lens->minor)
			lens->minor = file_lens->minor;
	}
	else
	{
		if (lens->size < file_lens->size)
			lens->size = file_lens->size;
	}
	if (lens->maj_min_siz < file_lens->maj_min_siz)
		lens->maj_min_siz = file_lens->maj_min_siz;
}

static void	print_lldir(file_t* dir)
{
	if (!(dir->content))
		return ;
	lens_t	lens;
	fill_byte(&lens, sizeof(lens_t), 0);
	file_t*	cnt = dir->content;
	long int	total = 0;
	while (cnt)
	{
		if (!cnt->info)
			load_file_info(cnt);
		if (cnt->info)
		{
			total += cnt->stats.st_blocks;
			update_lens(&lens, &(cnt->info->lens), cnt->info->file_type);
		}
		cnt = cnt->next;
	}
	char*	stotal = ft_litoa(total / 2);
	size_t	bufflen = 7 + ft_strlen(stotal);
	char*	buffer = malloc(bufflen);
	ft_memcpy(buffer, "total ", 6);
	ft_memcpy(&(buffer[6]), stotal, bufflen - 7);
	free(stotal);
	buffer[bufflen - 1] = '\n';
	write(1, buffer, bufflen);
	free(buffer);
	cnt = dir->content;
	while (cnt)
	{
		if (cnt->info)
		{
			print_llfile(cnt, &lens);
		}
		else
			printf("can't open file\n");
		cnt = cnt->next;
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
			else
				print_lldir(file);
		}
		else
		{
			file->name = file->path;
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
