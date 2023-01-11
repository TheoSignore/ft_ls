#ifndef LONG_LISTING_H
# define LONG_LISTING_H
# include "str_utils.h"
# include <stddef.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/sysmacros.h>
# include <grp.h>
# include <pwd.h>

typedef struct s_paddings
{
	size_t	len_nlinks;
	size_t	len_owner_name;
	size_t	len_group_name;
	size_t	len_size;
}	paddings_t;

typedef struct s_file_listing
{
	char					file_type;
	char					perms[9];
	size_t					number_of_links;
	char*					owner_name;
	char*					group_name;
	unsigned int			minor;
	unsigned int			major;
	size_t					size;
	size_t					blksize;
	char					mtime[13];
	char*					filename;
	char*					target;
	struct s_file_listing*	next;
	size_t					list_size;
}	file_listing_t;

file_listing_t*	create_file_listing(struct stat* file_stat, char* path, char* filename);
void			destroy_file_listing(file_listing_t* flst);

#endif
