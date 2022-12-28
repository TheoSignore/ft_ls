SRCS	= main.c \

OBJS	= ${SRCS:.c=.o}

BNS		= ${SRCBNS:.c=.o}

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
		gcc ${FLAGS} ${NAME} ${OBJS}

re:		fclean all

.PHONY:		all clean fclean re
