CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = tftp_server

SRC = main.c common.c connect.c get_file.c put_file.c
OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c server_main.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)
