#include "ft_ls.h"


file_t*	create_new_file(char* file_path)
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

static int	compare_name(char* a, char* b, int way)
{
	if (way)
		return (ft_strcmp(b, a));
	return (ft_strcmp(a, b));
}

static int	compare_time(time_t a, time_t b, int way)
{
	if (way)
	{
		if (b < a)
			return (1);
		return (-1);
	}
	if (b >= a)
		return (1);
	return (-1);
}

static char	compare(file_t* a, file_t* b, char options) // <
{
	if (!b)
		return (1);
	int	diff;
	if (options & SORT_MTIME)
	{
		if (a->stats.st_nlink == 0)
			lstat(a->path, &(a->stats));
		if (b->stats.st_nlink == 0)
			lstat(b->path, &(b->stats));
		diff = compare_time(a->stats.st_mtime, b->stats.st_mtime, options & REVERSE);
	}
	else
		diff = compare_name(a->name, b->name, options & REVERSE);
	if (diff < 0)
		return (1);
	return (0);
}

static void	insert_file(file_t** list, file_t* el, char options)
{
	file_t*	a = NULL;
	file_t*	b = *list;
	while (1)
	{
		if (compare(el, b, options))
		{
			if(a)
				a->next = el;
			else
				*list = el;
			el->next = b;
			return ;
		}
		a = b;
		b = b->next;
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

void	free_file(file_t* fp)
{
	if (fp)
	{
		free(fp->path);
		free_file_chain(fp->content);
		if (fp->info)
		{
			free(fp->info->number_of_links);
			free(fp->info->owner_name);
			free(fp->info->group_name);
			free(fp->info->minor);
			free(fp->info->major);
			free(fp->info->size);
			free(fp->info->blksize);
			free(fp->info->target);
			free(fp->info);
		}
		free(fp);
	}
}


void	get_content(file_t* dir, char options)
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
				break ;
			insert_file(&(dir->content), file, options);
		}
		errno = 0;
		subfile = readdir(dir_stream);
	}
	closedir(dir_stream);
	if (errno != 0)
		free_file(dir->content);
}
