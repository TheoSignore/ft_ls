// C.L.A.P.
// Command Line Arguments Parser

// option -> "l" | "env-file"
// value -> NULL | "srcs/.env"
typedef struct	opt_s
{
	char*	option;
	char*	value;
}	opt_t;

typedef struct	opts_s
{
	opt_t*	options;
	size_t	number;
}	opts_t;

opts_t*	clap(char** available_options, int ac, char** av);
// available_options -> ["l", "r", "env-file*"]
// if the option must be attached to a value, specify a '*' at the end

void*	get_option(opts* options, char* requested_option);
/* if the result is NULL, the requested option is not used
** if the result is "options", the option is used, but does not have any value
** else, the result is a char* pointing to the value of the option
**
** <cmd> --env-file=.env -l
**
** get_option(options, "r") -> NULL
** get_option(options, "l") -> options
** get_option(options, "env-file") -> ".env"
*/
