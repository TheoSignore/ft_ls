#include "ft_ls.h"

size_t	ft_strlen(const char* str)
{
	size_t	res = 0;

	if (str)
		while (str[res])
			res++;
	return (res);
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

int	ft_strcmp(char* str1, char* str2)
{
	while (*str1 && *str2)
	{
		if (*str1 != *str2)
			break;
		str1++;
		str2++;
	}
	return (*str1 - *str2);
}

char*	join_path(char* str1, char* str2)
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

void	ft_memcpy(void* dst, void* src, size_t sz)
{
	char*	src_dst = dst;
	char*	src_src = src;

	for (size_t i = 0 ; i < sz ; i++)
		src_dst[i] = src_src[i];
}

void	fill_byte(void* ptr, size_t n, char c)
{
	char*	cptr = ptr;
	for (size_t i = 0 ; i < n ; i++)
		cptr[i] = c;
}

void	reverse_memcpy(void* dst, void* src, size_t n)
{
	char*	cdst = dst;
	char*	csrc = src;
	n--;
	while (1)
	{
		*cdst = csrc[n];
		cdst--;
		if (n == 0)
			break;
		n--;
	}
}
