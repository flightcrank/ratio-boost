NAME =		ratio-boost

CC =		gcc

CFLAGS =	-I inc/ -lcurl -export-dynamic `pkg-config gtk+-3.0 --cflags --libs`

INC_DIR =	inc

INC_FILE =	blex.h \
		hash.h \
		list.h \
		urle.h
		

SRC_DIR =	src

SRC_FILE =	blex.c \
		gtk_main.c \
		hash.c \
		list.c \
		urle.c

OBJ_DIR =	.obj
OBJ_FILE =	$(SRC_FILE:.c=.o)

CRT_DIR =	.

SRC = 		$(addprefix $(SRC_DIR)/,$(SRC_FILE))
INC = 		$(addprefix $(INC_DIR)/,$(INC_FILE))
OBJ = 		$(addprefix $(OBJ_DIR)/,$(OBJ_FILE))
CRT = 		$(addprefix $(OBJ_DIR)/,$(CRT_DIR))

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC) Makefile
	@mkdir -p $(CRT) 2> /dev/null || true
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

install: $(NAME)
	mkdir /usr/share/$(NAME) || true
	cp ratio-boost.glade /usr/share/$(NAME)/.
	mv $(NAME) /usr/local/bin/.

uninstall:
	rm -r /usr/share/$(NAME) || true
	rm /usr/local/bin/$(NAME) || true
	
clean:
	rm -f $(OBJ)
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

