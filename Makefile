SRCS	= main.c \
		  time_stuff.c \
		  file_tree.c \
		  str_utils.c \
		  long_listing.c

OBJS	= ${SRCS:.c=.o}

#FLAGS	=  -Wall -Wextra -Werror
FLAGS	=  -Wall -Wextra -Werror -g
#FLAGS	=  -Wall -Wextra -Werror -g -fsanitize=address

NAME	= ft_ls

.c.o:
		gcc ${FLAGS} -c $< -o ${<:.c=.o}

all:		${NAME}

clean:
		rm -f ${OBJS} ${BNS}

fclean:		clean
		rm -f ${NAME}

${NAME}:	${OBJS}
		gcc ${FLAGS} -o ${NAME} ${OBJS}

re:		fclean all

.PHONY:		all clean fclean re
