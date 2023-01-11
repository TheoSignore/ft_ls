#include "str_utils.h"
//test
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

char*	ft_strdup(const char* str)
{
	if (!str)
		return (NULL);
	size_t	str_len = ft_strlen(str);
	char*	res = malloc(str_len + 1);
	if (res)
	{
		for (size_t i = 0 ; i < str_len ; i++)
			res[i] = str[i];
		res[str_len] = '\0';
	}
	return (res);
}

char*	ft_itoa(int n)
{
	size_t	sbuf = 10;
	char	buffer[12];

	for (int i = 0 ; i < 12 ; i++)
		buffer[i] = '\0';

	char	sign = n < 0 ? 0 : 1;
	long int	li = n < 0 ? -n : n;
	while (li > 0)
	{
		buffer[sbuf] = (li % 10) + '0';
		li /= 10;
		sbuf--;
	}
	if (!sign)
		buffer[sbuf] = '-';
	else
		sbuf++;
	return (ft_strdup(&(buffer[sbuf])));
}
