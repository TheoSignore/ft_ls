#include <stdlib.h>
#include <unistd.h>
#include <sys/sysmacros.h>
#include <pwd.h>
#include <grp.h>
#include "time_stuff.h"
#include "file_tree.h"
#include <sys/types.h>



int	main(int ac, char** av)
{
	if (ac != 2)
		return (1);
	file_tree_node_t*	thing = create_file_tree(av[1], 1);
	func(NULL, thing);
	print_other_file_tree(thing);
	return (0);
}
//11 + 1 + max_len(number_of_hard_links) + 1 + max_len(uname) + 1 + max_len(gname) + 1 + max_len(size) + 1 + 12 + 1 + max_len(file_name);
