NAME = libft.a

CC = cc
CFLAGS = -Wall -Wextra -Werror

SOURCES = ft_strlen.c ft_strdup.c ft_strjoin.c ft_split.c ft_strcmp.c ft_strncmp.c ft_strchr.c ft_substr.c ft_memset.c ft_putstr_fd.c ft_putendl_fd.c ft_atoi.c ft_isalnum.c ft_itoa.c ft_isalpha.c ft_strrchr.c ft_strcpy.c ft_strcat.c

OBJS = $(SOURCES:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
