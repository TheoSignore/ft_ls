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
# include <time.h>

typedef struct s_lens
{
	size_t	nlinks;
	size_t	oname;
	size_t	gname;
	size_t	major;
	size_t	minor;
	size_t	size;
	size_t	maj_min_siz;

} lens_t;

typedef struct s_fileinfo
{
	char	file_type;
	char	perms[9];
	char*	number_of_links; //zi
	char*	owner_name;
	char*	group_name;
	char*	minor; //ui
	char*	major; //ui
	char*	size; //zi
	char*	blksize; //zi
	char	mtime[12];
	char*	target;
	lens_t	lens;
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

typedef	struct my_time_s
{
	int	year;
	int	month;
	int	day;
	int	seconds;
}	mytime_t;

/* parsing.h */
int		remove_options_and_name(int ac, char** av);
char	get_options(int ac, char** av);

/* str_utils.c */
size_t	ft_strlen(const char* str);
char*	ft_strdup(const char* str);
int		ft_strcmp(char* str1, char* str2);
char*	join_path(char* str1, char* str2);
void	ft_memcpy(void* dst, void* src, size_t sz);
void	fill_byte(void* ptr, size_t n, char c);
void	reverse_memcpy(void* dst, void* src, size_t n);

/* file_and_dir.c */
file_t*	create_new_file(char* file_path);
void	free_file(file_t* fp);
void	get_content(file_t* dir, char options);

/* time_stuff.c */
void	extract_and_format_time(time_t mtim, char* str_mtim);

/* info.c */
int	load_file_info(file_t* file);

/* ntoa.c */
char*	ft_litoa(long int n);
char*	ft_luitoa(long unsigned int n);
char*	ft_uitoa(unsigned int n);

/* buffering_and_printing.c */
void	display_dir(file_t* file, char options, int flag);


void	access_error(char* path);

#endif
