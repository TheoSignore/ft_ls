#ifndef STR_UTILS_H
	#define STR_UTILS_H
	#include <stdlib.h>

	size_t		ft_strlen(char const *s);
	const char*	ft_strchr(const char *s, char c);
	char		ft_strncmp(const char *s1, const char *s2, size_t n);
	char*		ft_strdup(const char* str);
	char*		ft_itoa(int n);

#endif
