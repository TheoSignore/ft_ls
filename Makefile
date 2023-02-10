SRCS	:=	main.c \
			parsing.c \
			str_utils.c \
			file_and_dir.c \
			time_stuff.c \
			info.c \
			ntoa.c \
			buffering_and_printing.c

OBJS	=	${SRCS:%.c=.%.o}

DEP		=	${SRCS:%.c=.%.d}

#FLAGS	:=  -Wall -Wextra -Werror
FLAGS	:=  -Wall -Wextra -Werror -g -MMD
#FLAGS	:=  -Wall -Wextra -Werror -g -fsanitize=address

COMPILO	:= gcc

NAME	:=	ft_ls

.SUFFIXES: .c .o .d

.%.o: %.c
		${COMPILO} ${FLAGS} -c $< -o $@

all:	${NAME}

clean:
		rm -f ${OBJS}
		rm -f ${DEP}

fclean:		clean
		rm -f ${NAME}

${NAME}: ${OBJS}
		${COMPILO} ${FLAGS} -o ${NAME} ${OBJS}

-include ${DEP}

re:		fclean all

.PHONY:		all clean fclean re dependencies
