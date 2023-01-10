#include "file_tree.h"

static char*	join_path(char* str1, char* str2)
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

static void	push_front_subfile(file_tree_node_t* parent, file_tree_node_t* sf)
{
	if (parent->subfile)
		sf->next_file = parent->subfile;
	parent->subfile = sf;
}

static file_tree_node_t*	create_file_tree_node(char* file_path)
{
	file_tree_node_t*	res = malloc(sizeof(file_tree_node_t));
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

file_tree_node_t*	create_file_tree(char* file_path, char flag)
{
	file_tree_node_t*	new_entry = create_file_tree_node(file_path);
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
			file_tree_node_t*	new_sub_entry;
			if (subfile->d_name[0] != '.' || flag == 2)
			{
				new_sub_entry = create_file_tree(join_path(new_entry->filename, subfile->d_name), 1);
				push_front_subfile(new_entry, new_sub_entry);
			}
			subfile = readdir(dirptr);
		}
		closedir(dirptr);
	}
	return (new_entry);
}

static size_t	number_of_files(file_tree_node_t* el)
{
	size_t	res = 0;
	while (el)
	{
		res++;
		el = el->next_file;
	}
	return (res);
}

void	print_file_tree(file_tree_node_t* root)
{
	size_t	subfile_number = number_of_files(root->subfile);
	if (subfile_number == 0)
		printf("%s ", root->filename);
	else
		printf ("\n%s (%zu)\n\t", root->filename, subfile_number);
	file_tree_node_t*	ptr = root->subfile;
	while (ptr)
	{
		print_file_tree(ptr);
		ptr = ptr->next_file;
	}
}
