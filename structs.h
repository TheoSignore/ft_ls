

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
	char*			file_name;
	struct stat		file_stats;
	struct s_file*	next;
	fileinfo_t		*info;
}	file_t;

typedef struct s_directory
{
	char*				path;
	struct stat			dir_stats;
	struct s_directory*	next;
	file_t*				content;
	lens_t				lens;
	fileinfo_t*			info;
}	directory_t;
