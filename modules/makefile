CC = gcc
CFLAGS = -Wall -fPIC -I../include
LDFLAGS = -lwiringPi
LOGGER_SRC = ../server/logger.c

MODULES = led_mod buzzer_mod 7seg_mod cds_mod

all: $(MODULES:%=lib%.so)

lib%.so: %.c
	$(CC) $(CFLAGS) -shared -o $@ $< $(LOGGER_SRC) $(LDFLAGS)

clean:
	rm -f *.so