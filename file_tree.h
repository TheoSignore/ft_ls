#ifndef FILE_TREE_H
# define FILE_TREE_H
# include "str_utils.h"
# include <sys/stat.h>
# include <dirent.h>
# include <stdio.h>

typedef struct s_file_tree_node
{
	char*						path;
	char*						filename;
	struct stat					file_stats;
	struct s_file_tree_node*	next_file;
	struct s_file_tree_node*	subfile;
}	file_tree_node_t;

file_tree_node_t*	create_file_tree(char* file_path, char flag);
void				print_file_tree(file_tree_node_t* root);

#endif
