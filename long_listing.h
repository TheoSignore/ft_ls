#ifndef LONG_LISTING_H
# define LONG_LISTING_H

typedef struct s_file_listing
{
	char			file_type;
	char			perms[9];
	size_t			number_of_links;
	char*			owner_name;
	char*			group_name;
	unsigned int	minor;
	unsigned int	major;
	size_t			size;
	size_t			blksize;
	char			mtime[13];
}	file_listing_t;



#endif
