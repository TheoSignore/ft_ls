#ifndef FT_LS_H
# define FT_LS_H

# define LONG_LISTING 1
# define RECURSIVE 2
# define ALL 4
# define REVERSE 8
# define SORT_MTIME 16
# define UNKNOWN 128
# define BUFFER_SIZE 80

# include <stdlib.h>
# include <unistd.h>
# include <sys/stat.h>
# include <dirent.h>
# include <errno.h>
# include <sys/sysmacros.h>
# include <pwd.h>
# include <grp.h>
# include <sys/types.h>
# include <stdio.h>

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

/* parsing.h */
int		remove_options_and_name(int ac, char** av);
char	get_options(int ac, char** av);

/* str_utils.c */
size_t	ft_strlen(const char* str);
char*	ft_strdup(const char* str);
int		ft_strcmp(char* str1, char* str2);
char*	join_path(char* str1, char* str2);
void	ft_memcpy(void* dst, void* src, size_t sz);

/* file_and_dir.c */
file_t*	create_new_file(char* file_path);
void	free_file(file_t* fp);
void	get_content(file_t* dir, char options);

/* time_stuff.c */
void	extract_and_format_time(time_t mtim, char* str_mtim);
#endif
