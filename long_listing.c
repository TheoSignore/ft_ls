#include "long_listing.h"
#include "time_stuff.h"

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

file_listing_t*	create_file_listing(struct stat* file_stat, char* path, char* filename)
{
	file_listing_t*	res = malloc(sizeof(file_listing_t));
	if (!res)
		return (NULL);
	res->file_type = get_type(file_stat->st_mode);
	get_perms(file_stat->st_mode, res->perms);
	res->number_of_links = file_stat->st_nlink;
	{
		struct passwd*	spwd = getpwuid(file_stat->st_uid);
		if (spwd)
			res->owner_name = ft_strdup(spwd->pw_name);
		else
			res->owner_name = ft_itoa(file_stat->st_uid);
	}
	{
		struct group*	grp = getgrgid(file_stat->st_gid);
		if (grp)
			res->group_name = ft_strdup(grp->gr_name);
		else
			res->group_name = ft_itoa(file_stat->st_gid);
	}
	res->minor = minor(file_stat->st_dev);
	res->major = major(file_stat->st_dev);
	res->size = file_stat->st_size;
	res->blksize = file_stat->st_blksize;
	extract_and_format_time(file_stat->st_mtime, (char*)&(res->mtime));
	res->filename = ft_strdup(filename);
	if (res->file_type == 'l')
		res->target = get_link_target(path);
	else
		res->target = NULL;
	res->next = NULL;
	res->list_size = 0;
	return (res);
}

void	destroy_file_listing(file_listing_t* flst)
{
	free(flst->owner_name);
	free(flst->group_name);
	free(flst->filename);
	free(flst->target);
	flst->owner_name = NULL;
	flst->group_name = NULL;
	flst->filename = NULL;
	flst->target = NULL;
	if (flst->next)
		destroy_file_listing(flst->next);
	flst->next = NULL;
}

size_t	get_listing_len(file_listing_t* flst, paddings_t* pds)
{
	size_t	res = 10 + 1
		+ pds->len_nlinks + 1
		+ pds->len_owner_name + 1
		+ pds->len_group_name + 1
		+ pds->len_size + 1
		+ 13 + 1
		+ ft_strlen(flst->filename) + 1;
	if (flst->target)
		res += 1 + 2 + 1 + ft_strlen(flst->target);
	flst->list_size = res;
	return (res);
}

