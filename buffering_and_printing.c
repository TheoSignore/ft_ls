#include "ft_ls.h"

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


static void	init_lsbf(lsbf_t* lsbf, char* bf, lens_t* lens)
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

static void	write_info_to_buffer(char* buff, file_t* file, lens_t* lens)
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
		access_error(file->path);
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
	lens_t	lens;
	if (dir->content)
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
		print_llfile(cnt, &lens);
		cnt = cnt->next;
	}
}

//trwxrwxrwx <hardlinks> <uname> <gname> <<major>, <minor>|<size>> XXX_XX_XXXXX <name>[ -> target]\n
//|_________|           |       |       |                         |____________|                  |
// 11                   1       1       1                            14                           1
// 11 + 1 + 1 + 1 + 14 + 1 = 29
// 29 + hl_len + uname_len + gname + (major_len + 2 + major _len|size_len) + filename_len [+ 2 + target_len]

void	display_dir(file_t* file, char options, int flag)
{
	if (file)
	{
		if (S_ISDIR(file->stats.st_mode))
		{
			if (flag || options & RECURSIVE)
			{
				size_t	len = ft_strlen(file->path);
				char	buffer[len + 2];
				buffer[len] = ':';
				buffer[len + 1] = '\n';
				ft_memcpy(buffer, file->path, len);
				write(1, buffer, len + 2);
			}
			if (options & LONG_LISTING)
				print_lldir(file);
			else
				print_content(file->content);
		}
		else
		{
			file->name = file->path;
			if (!(options & LONG_LISTING))
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
