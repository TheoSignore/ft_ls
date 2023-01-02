#include "clap.h"

arg_t*	clap(char** available_options, int ac, char** av)
{
	if (ac <= 1 || !available_options || !av)
		return (NULL);
	while (*available_options)
	{
		
		available_options++;
	}
}
