CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = tftp_client

SRC = main.c connect.c download.c end.c help.c mode.c upload.c
OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c main.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)
