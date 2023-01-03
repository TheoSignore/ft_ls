#include "str_utils.h"

size_t	ft_strlen(char const *s)
{
	size_t	size = 0;
	if (s)
	{
		while (s[size])
			size++;
	}
	return (size);
}

const char	*ft_strchr(const char *s, char c)
{
	if (!s)
		return ( NULL );
	size_t	i = 0;
	while (s[i] != c && s[i] != '\0')
		i++;
	return (s[i] == c ? &(s[i]) : NULL );
}

char	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	lens1 = ft_strlen(s1) + 1;
	size_t	lens2 = ft_strlen(s2) + 1;
	size_t	i = 0;
	while(i < n && i < lens1 && i < lens2)
	{
		if (s1[i] != s2[i])
			return (0);
		i++;
	}
	return (i >= n ? 1 : 0);
}
