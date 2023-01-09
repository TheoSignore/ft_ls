#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <pwd.h>
#include <grp.h>
#include "time_stuff.h"
#include <sys/types.h>
#include <dirent.h>


size_t	ft_strlen(char const *s)
{
	size_t	size = 0;
	if (s)
	{
		while (s[size])
			size++;
	}
	return (size);
}

//	static char	get_type(mode_t mode)
//	{
//		switch (mode & S_IFMT)
//		{
//			case S_IFREG:
//				return '-';
//			case S_IFDIR:
//				return 'd';
//			case S_IFLNK:
//				return 'l';
//			case S_IFSOCK:
//				return 's';
//			case S_IFBLK:
//				return 'b';
//			case S_IFCHR:
//				return 'c';
//			case S_IFIFO:
//				return 'p';
//			default:
//				return '?';
//		}
//	}
//	
//	static void	get_perms(mode_t mode, char* buf)
//	{
//		buf[0] = (mode & S_IRUSR) ? 'r' : '-';
//		buf[1] = (mode & S_IWUSR) ? 'w' : '-';
//		buf[2] = (mode & S_IXUSR) ? 'x' : '-';
//		buf[3] = (mode & S_IRGRP) ? 'r' : '-';
//		buf[4] = (mode & S_IWGRP) ? 'w' : '-';
//		buf[5] = (mode & S_IXGRP) ? 'x' : '-';
//		buf[6] = (mode & S_IROTH) ? 'r' : '-';
//		buf[7] = (mode & S_IWOTH) ? 'w' : '-';
//		buf[8] = (mode & S_IXOTH) ? 'x' : '-';
//	}

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

void	ft_memcpy(void* ptr1, void* ptr2, size_t n)
{
	char*	cptr1 = ptr1;
	char*	cptr2 = ptr2;
	for (size_t i = 0 ; i < n ; i++)
		cptr2[i] = cptr1[i];
}

typedef struct s_lsel
{
	char*			path;
	char*			filename;
	struct stat		file_stats;
	struct s_lsel*	next_file;
	struct s_lsel*	subfile;
}	lsel_t;

static void	push_front_subfile(lsel_t* parent, lsel_t* sf)
{
	if (parent->subfile)
		sf->next_file = parent->subfile;
	parent->subfile = sf;
}

//	static void	push_front_next_file(lsel_t* f, lsel_t* nf)
//	{
//		if (f->next_file)
//			nf->next_file = f->next_file;
//		f->next_file = nf;
//	}

static lsel_t*	create_entry(char* file_path)
{
	lsel_t*	res = malloc(sizeof(lsel_t));
	if (res)
	{
		res->path = file_path;
		size_t	i = 0;
		while (file_path[i])
			i++;
		while (i && i != '/')
			i--;
		if (i)
			i++;
		res->filename = &(file_path[i]);
		res->next_file = NULL;
		res->subfile = NULL;
	}
	return res;
}

//char*	ft_strdup(char* str)
//{
//	size_t	len = ft_strlen(str);
//	char*	res = malloc(len + 1);
//	if (res)
//	{
//		for (size_t i = 0 ; i < len ; i++)
//			res[i] = str[i];
//		res[len] = '\0';
//	}
//	return (res);
//}

char*	join_path(char* str1, char* str2)
{
	size_t	len1 = ft_strlen(str1);
	size_t	len2 = ft_strlen(str2);
	size_t	len_res = len1 + len2 + (str1[len1 - 1] == '/' ? 1 : 2);
	char*	res = malloc(len_res);
	if (!res)
		return (NULL);
	for (size_t i = 0 ; i < len1 ; i++)
	{
		*res = str1[i];
		res++;
	}
	if (str1[len1 - 1] != '/')
	{
		*res = '/';
		res++;
	}
	for (size_t i = 0 ; i <= len2 ; i++)
	{
		*res = str2[i];
		res++;
	}
	res -= len_res;
	return (res);
}

static lsel_t*	make_new_entry(char* file_path, char flag)
{
	lsel_t*	new_entry = create_entry(file_path);
	if (!new_entry)
		return NULL;
	int res = 0;
	if (flag == 0)
		res = stat(file_path, &(new_entry->file_stats));
	else
		res = lstat(file_path, &(new_entry->file_stats));
	(void)res;
	if (res)
	{
		perror(new_entry->filename);
		exit(1);
	}
	if (S_ISDIR(new_entry->file_stats.st_mode))
	{
		DIR*	dirptr = opendir(new_entry->filename);
		struct dirent* subfile = readdir(dirptr);
		subfile = readdir(dirptr);
		subfile = readdir(dirptr);
		while (subfile)
		{
			lsel_t*	new_sub_entry;
			if (subfile->d_name[0] != '.' || flag == 2)
			{
				new_sub_entry = make_new_entry(join_path(new_entry->filename, subfile->d_name), 1);
				push_front_subfile(new_entry, new_sub_entry);
			}
			subfile = readdir(dirptr);
		}
		closedir(dirptr);
	}
	return (new_entry);
}

size_t	lsel_len(lsel_t* el)
{
	size_t	res = 0;
	while (el)
	{
		res++;
		el = el->next_file;
	}
	return (res);
}

void	print_lsel(lsel_t* root)
{
	size_t	subfile_number = lsel_len(root->subfile);
	if (subfile_number == 0)
		printf("%s ", root->filename);
	else
		printf ("\n%s (%zu)\n\t", root->filename, subfile_number);
	lsel_t*	ptr = root->subfile;
	while (ptr)
	{
		print_lsel(ptr);
		ptr = ptr->next_file;
	}
}

//Wed Jun 30 21:49:08 1993\n
//0   4   8  11       20
//    0   4  7        16
int	main(int ac, char** av)
{
	if (ac != 2)
		return (1);
	struct stat	stats;
	if (stat(av[1], &stats))
	{
		perror("FAILED");
		return (1);
	}
	lsel_t*	thing = make_new_entry(av[1], 1);
	print_lsel(thing);
	return (0);
}
//11 + 1 + max_len(number_of_hard_links) + 1 + max_len(uname) + 1 + max_len(gname) + 1 + max_len(size) + 1 + 12 + 1 + max_len(file_name);
