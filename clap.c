#include "clap.h"

static void*	ft_malloc(size_t n)
{
	void*	res = malloc(n);
	if (!res)
		perror("Failed");
	return (res);
}

static void	destroy_arg(arg_t* arg)
{
	free(arg->option);
	arg->option = NULL
	arg->value = NULL;
	arg->next = NULL;
}

static void	destroy_args(arg_t** list)
{
	arg_t*	tmp;

	while (*list)
	{
		tmp = *list;
		*list = (*list)->next;
		destroy_arg(tmp);
	}
}

static arg_t*	create_arg(void)
{
	arg_t*	res = ft_malloc(sizeof(arg_t));
	if (res)
	{
		res->option = NULL;
		res->value = NULL;
		res->next = NULL;
	}
	return res;
}

static void	extract_argument(char* str, char** options, arg_t** res)
{
	arg_t*	argument = create_arg();
	if (!argument)
		return (destroy_args(res));
}

arg_t*	clap(char** available_options, int ac, char** av)
{
	if (ac <= 1 || !available_options || !av)
		return (NULL);
	arg_t*	res = NULL;
	for (int i = 0 ; i < ac ; i++)
	{
		extract_agument(av[i], available_options, &res);
		if (!res)
			return NULL;
	}
}
