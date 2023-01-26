#ifndef FT_LS_H
# define FT_LS_H

# define LONG_LISTING 1
# define RECURSIVE 2
# define ALL 4
# define REVERSE 8
# define SORT_MTIME 16
# define UNKNOWN 128

# include <stdlib.h>
# include <unistd.h>
# include <sys/stat.h>
# include <dirent.h>
# include <errno.h>
# include <sys/sysmacros.h>
# include <pwd.h>
# include <grp.h>
# include <sys/types.h>

typedef struct s_lens
{
	size_t	nlinks;
	size_t	owner_name;
	size_t	group_name;
	size_t	size;
} lens_t;

typedef struct s_fileinfo
{
	char	file_type;
	char	perms[9];
	size_t	number_of_links; //zi
	char*	owner_name;
	char*	group_name;
	char*	minor; //ui
	char*	major; //ui
	char*	size; //zi
	char*	blksize; //zi
	char	mtime[13];
	char*	target;
	size_t	extra_len;
}	fileinfo_t;

typedef struct s_file
{
	char*			path;
	char*			name;
	struct stat		stats;
	struct s_file*	content;
	lens_t			lens;
	fileinfo_t*		info;
	struct s_file*	next;
}	file_t;

#endif
