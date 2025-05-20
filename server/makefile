CC = gcc
CFLAGS = -Wall -I../include
LDFLAGS = -ldl -lwiringPi

TARGET = main_server
SRC = main_server.c mod_loader.c logger.c     # logger.c 추가!

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
