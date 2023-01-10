
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

file_listing_t*	create_file_listing(struct stat* file_stat)
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
			res->owner_name = itoa(file_stat->st_uid);
	}
	{
		struct group*	grp = getgrgid(file_stat->st_gid);
		if (grp)
			res->group_name = ft_strdup(grp->gr_name);
		else
			res->group_name = itoa(file_stat->st_gid);
	}
	res->minor = minor(file_stat->st_dev);
	res->major = major(file_stat->st_dev);
	res->size = file_stat->st_size;
	res->blksize = file_stat->st_blksize;
	extract_and_format_time(file_stat->mtime, &(res->mitme));
	return (res);
}

/*
	char	buf[11];
	buf[10] = '\0';
	buf[0] = get_type(stats.st_mode);
	get_perms(stats.st_mode, &(buf[1]));
	printf(
		"%s %lu %s %s",
		buf,
		stats.st_nlink,
		(getpwuid(stats.st_uid)->pw_name),
		(getgrgid(stats.st_gid)->gr_name)
	);
	if (stats.st_rdev)
		printf(" %u, %u", major(stats.st_rdev), minor(stats.st_rdev));
	else
		printf(" %li", stats.st_size);
	char	mtime[13];
	extract_and_format_time(stats.st_mtime, mtime);
	printf(
		" %s %s\n",
		mtime,
		av[1]
	);
*/
