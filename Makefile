CC = clang
CFLAGS = -Wall -Wpedantic -O2
INC = include
SRC = main.c $(INC)/cee/cmds.c $(INC)/cee/types.c $(INC)/cee/configparser.c $(INC)/utils/logging.c $(INC)/utils/stringutils.c $(INC)/utils/fileutils.c
OBJ = $(SRC:.c=.o)
TARGET = main

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
