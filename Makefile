CC = gcc
CFLAGS = -Wall -Wextra -Werror

SRC = main.c ping.c utils.c
OBJ = $(SRC:.c=.o)
NAME = ft_ping

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re