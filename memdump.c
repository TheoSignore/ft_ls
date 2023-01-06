#include <sys/xattr.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

char lel = 0;
void	print_bits(char c)
{
	char	bits;

	bits = 8;
	char cpy = c;
	if (c >= ' ' && c <= '~')
		write(1, "\033[30;47m", 8);
	while (bits > 0)
	{
		if (c & 128)
			write(1, "1", 1);
		else
			write(1, "0", 1);
		c <<= 1;
		bits--;
	}
	if (cpy >= ' ' && cpy <= '~')
		write(1, "\033[0m", 4);
	write (1, " ", 1);
	lel++;
}

int main(int ac , char** av)
{
	if (ac != 2)
		return (1);
	char	buf1[1000];
	char	buf2[1000];
	size_t	lol = listxattr(av[1], buf1, 1000);
	printf("%s\n", av[1]);
	if (!lol || lol > 1000 )
	{
		if (errno == ENOTSUP)
			printf("FUCK\n");
		if (lol)
			perror("FAILED");
		return (0);
	}
	printf(" (%zu)\n", write(1, buf1, lol));
	lol = getxattr(av[1], buf1, buf2, 1000);
	printf(" (%zu)\n", write(2, buf2, lol));
	(void)lol;
	char	ascii_s[7];
	ascii_s[6] = '\0';
	ascii_s[5] = '\'';
	ascii_s[0] = '\'';
	for(size_t i = 1 ; i < 5 ; i++)
		ascii_s[i] = '-';
	size_t	nbc = 0;
	int	toint = 0;
	char*	intptr = (char*)&toint;
	for (size_t i = 0 ; i < lol ; i++)
	{
		print_bits(buf2[i]);
		intptr[3 - (lel - 1)] = buf2[i];
		if (buf2[i] >= ' ' && buf2[i] <= '~')
		{
			ascii_s[nbc + 1] = buf2[i];
			nbc++;
		}
		if (lel >= 4)
		{
			write(1, ascii_s, 7);
			printf(" [%zu] (%i)\n", nbc, toint);
			nbc = 0;
			for(size_t i = 1 ; i < 5 ; i++)
				ascii_s[i] = '-';
			lel = 0;
		}
	}
	return (0);
}
