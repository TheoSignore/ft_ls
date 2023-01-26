#include "file_tree.h"


static file_t*	create_new_file(char* file_path)
{
	file_t*	res = malloc(sizeof(file_t));
	if (res)
	{
		res->path = file_path;

		while (*file_path);
			file_path++;
		while (res->path != file_path && *file_path != '/')
			file_path--;
		if (res->path != file_path)
			file_path++;
		res->name = file_path;

		res->stats.st_nlink = 0;
		res->content = NULL;
		res->content = NULL;
		res->info = NULL;
		res->next = NULL;
	}
	else
		perror("ft_ls:");
	return res;
}


file_t*	func(lst_t* args)
{
	file_t*	foo = NULL;
	while (args)
	{
		file_t*	bar = create_new_file(args->str);
		if (stat(bar->path, &(bar->stats)))
		{
			access_error(bar->path);
			args = args->next;
			continue ;
		}
		if (!foo)
			foo = bar;
		else
		if (S_ISDIR(bar->st_mode))
		{
			get_directory_content(bar);
		}
		args = args->next;
	}
}
