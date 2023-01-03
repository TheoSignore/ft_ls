#ifndef CLAP_H
	#define CLAP_H

	// C.L.A.P.
	// Command Line Arguments Parser
	
	// option -> "l" | "-sf" | "--env-file="
	// value -> NULL | "srcs/.env"
	typedef struct	arg_s
	{
		char*			option;
		char*			value;
		struct arg_s*	next;
	}	arg_t;

	arg_t*	clap(char** available_options, int ac, char** av);
	// available_options -> ["-l", "-sf*", "--env-file=", ]
	// if the option must be followed by a value, specify a '*' at the end
	// if the option must be attached to a value, specify the '='

	void*	get_option(arg_t* options, char* requested_option);
	/* if the result is NULL, the requested option is not used
	** if the result is "options", the option is used, but does not have any value
	** else, the result is a char* pointing to the value of the option
	**
	** <cmd> --env-file=.env -l
	**
	** get_option(options, "-r") -> NULL
	** get_option(options, "-l") -> options
	** get_option(options, "--env-file=") -> ".env"
	*/

#endif
