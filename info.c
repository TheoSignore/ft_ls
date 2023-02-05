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

	if (mode & S_ISUID)
	{
		if (mode & S_IXUSR)
			buf[2] = 's';
		else
			buf[2] = 'S';
	}
	else if (mode & S_IXUSR)
		buf[2] = 'x';
	else
		buf[2] = '-';

	buf[3] = (mode & S_IRGRP) ? 'r' : '-';
	buf[4] = (mode & S_IWGRP) ? 'w' : '-';
	if (mode & S_ISGID)
	{
		if (mode & S_IXGRP)
			buf[5] = 's';
		else
			buf[5] = 'S';
	}
	else if (mode & S_IXGRP)
		buf[5] = 'x';
	else
		buf[5] = '-';

	buf[6] = (mode & S_IROTH) ? 'r' : '-';
	buf[7] = (mode & S_IWOTH) ? 'w' : '-';
	if (mode & S_ISVTX)
	{
		if (mode & S_IXOTH)
			buf[8] = 't';
		else
			buf[8] = 'T';
	}
	else if (mode & S_IXOTH)
		buf[8] = 'x';
	else
		buf[8] = '-';
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
	fill_byte(info, sizeof(fileinfo_t), 0);
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
	info->lens.nlinks = ft_strlen(info->number_of_links);
	info->lens.oname = ft_strlen(info->owner_name);
	info->lens.gname = ft_strlen(info->group_name);
	if (info->file_type == 'c' || info->file_type == 'b')
	{
		info->lens.major = ft_strlen(info->major);
		info->lens.minor = ft_strlen(info->minor);
		info->lens.maj_min_siz = info->lens.major + 2 + info->lens.minor;
	}
	else
	{
		info->lens.size = ft_strlen(info->size);
		info->lens.maj_min_siz = info->lens.size;
	}
	if (info->file_type == 'l')
		info->target = get_link_target(file->path);
	else
		info->target = NULL;
	file->info = info;
}
