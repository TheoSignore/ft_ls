#include "ft_ls.h"

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

static char*	get_owner(int uid)
{
	struct passwd*	spwd = getpwuid(uid);
	if (spwd)
		return (ft_strdup(spwd->pw_name));
	return (ft_uitoa(uid));
}

static char*	get_group(int gid)
{
	struct group*	grp = getgrgid(gid);
	if (grp)
		return (ft_strdup(grp->gr_name));
	return (ft_uitoa(gid));
}

void	load_file_info(file_t* file)
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
	info->number_of_links = ft_luitoa(file->stats.st_nlink);
	info->owner_name = get_owner(file->stats.st_uid);
	info->group_name = get_group(file->stats.st_gid);
	info->minor = ft_uitoa(minor(file->stats.st_rdev));
	info->major = ft_uitoa(major(file->stats.st_rdev));
	info->size = ft_litoa(file->stats.st_size);
	info->blksize = ft_litoa(file->stats.st_blksize);
	extract_and_format_time(file->stats.st_mtime, info->mtime);
	if (info->file_type == 'l')
		info->target = get_link_target(file->path);
	else
		info->target = NULL;
	file->info = info;
}
