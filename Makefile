SRCS	:=	main.c \
			parsing.c \
			str_utils.c \
			file_and_dir.c \
			time_stuff.c \
			info.c \
			ntoa.c

OBJS	=	${SRCS:.c=.o}

DEP		=	${SRCS:.c=.d}

#FLAGS	:=  -Wall -Wextra -Werror
FLAGS	:=  -Wall -Wextra -Werror -g
#FLAGS	:=  -Wall -Wextra -Werror -g -fsanitize=address

COMPILO	:= gcc

NAME	:=	ft_ls

.SUFFIXES: .c .o .d

.c.d:
		${COMPILO} -MM $< -MF ${<:.c=.d}

.c.o:
		${COMPILO} ${FLAGS} -c $< -o ${<:.c=.o}

all:	dependencies ${NAME}

clean:
		rm -f ${OBJS}
		rm -f ${DEP}

fclean:		clean
		rm -f ${NAME}

${NAME}: ${OBJS}
		${COMPILO} ${FLAGS} -o ${NAME} ${OBJS}

dependencies:
include ${DEP}

re:		fclean all

.PHONY:		all clean fclean re dependencies
