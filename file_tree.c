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

static void	add_back_next_file(file_tree_node_t* ftn, file_tree_node_t* nf)
{
	while (ftn->next_file)
		ftn = ftn->next_file;
	ftn->next_file = nf;
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
		while (i && file_path[i] != '/')
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
	if (res)
	{
		perror(file_path);
		return (new_entry);
	}
	if (S_ISDIR(new_entry->file_stats.st_mode))
	{
		DIR*	dirptr = opendir(new_entry->path);
		if (!dirptr)
			return (new_entry);
		struct dirent* subfile = readdir(dirptr);
		subfile = readdir(dirptr);
		subfile = readdir(dirptr);
		while (subfile)
		{
			file_tree_node_t*	new_sub_entry;
			if (subfile->d_name[0] != '.' || flag == 2)
			{
				new_sub_entry = create_file_tree(join_path(new_entry->path, subfile->d_name), 1);
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


file_tree_node_t*	dupftn(file_tree_node_t* ftn)
{
	file_tree_node_t*	res = malloc(sizeof(file_tree_node_t));
	if (res)
	{
		size_t	ptrdiff = ftn->filename - ftn->path;
		res->path = ft_strdup(ftn->path);
		res->filename = res->path + ptrdiff;
		ft_memcpy(&(ftn->file_stats), &(res->file_stats), sizeof(struct stat));
		res->next_file = NULL;
		res->subfile = ftn->subfile;
	}
	return (res);
}

void	func(file_tree_node_t* root, file_tree_node_t* ftn)
{
	if (root)
	{
		add_back_next_file(root, dupftn(ftn));
	}
	else
		root = ftn;
	file_tree_node_t*	ptr = ftn->subfile;
	while (ptr)
	{
		if (ptr->subfile)
			func(root, ptr);
		ptr = ptr->next_file;
	}
}

void	print_other_file_tree(file_tree_node_t* ftn)
{
	size_t	buffer_size = ft_strlen(ftn->path) + 2; // + ":\n"
	buffer_size += ftn->next_file ? 1 : 0; // additionnal LF
	file_tree_node_t*	ptr = ftn->subfile;
	while (ptr)
	{
		buffer_size += 1 + ft_strlen(ptr->filename);
		ptr = ptr->next_file;
	}
	char	*buffer = malloc(buffer_size);
	buffer[buffer_size - 1] = '\n';
	if (ftn->next_file)
		buffer[buffer_size - 2] = '\n';
	size_t	i = non_null_term_strcpy(ftn->path, buffer);
	buffer[i++] = ':';
	buffer[i++] = '\n';
	ptr = ftn->subfile;
	while (ptr)
	{
		i += non_null_term_strcpy(ptr->filename, &(buffer[i]));
		ptr = ptr->next_file;
		if (ptr)
			buffer[i++] = ' ';
	}
	write(1, buffer, buffer_size);
	if (ftn->next_file)
		print_other_file_tree(ftn->next_file);
}

void	print_other_file_tree(file_tree_node_t* ftn)
{
	size_t	buffer_size = ft_strlen(ftn->path) + 2; // + ":\n"
	buffer_size += ftn->next_file ? 1 : 0; // additionnal LF
	file_tree_node_t*	ptr = ftn->subfile;
	while (ptr)
	{
		buffer_size += 1 + ft_strlen(ptr->filename);
		ptr = ptr->next_file;
	}
	char	*buffer = malloc(buffer_size);
	buffer[buffer_size - 1] = '\n';
	if (ftn->next_file)
		buffer[buffer_size - 2] = '\n';
	size_t	i = non_null_term_strcpy(ftn->path, buffer);
	buffer[i++] = ':';
	buffer[i++] = '\n';
	ptr = ftn->subfile;
	while (ptr)
	{
		i += non_null_term_strcpy(ptr->filename, &(buffer[i]));
		ptr = ptr->next_file;
		if (ptr)
			buffer[i++] = ' ';
	}
	write(1, buffer, buffer_size);
	if (ftn->next_file)
		print_other_file_tree(ftn->next_file);
}
