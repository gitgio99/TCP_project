CC = gcc
CFLAGS = -Wall -fPIC -I../include
LDFLAGS = -lwiringPi

MODULES = led_mod buzzer_mod 7seg_mod cds_mod

all: $(MODULES:%=lib%.so)

lib%.so: %.c
	$(CC) $(CFLAGS) -shared -o $@ $< $(LDFLAGS)

clean:
	rm -f *.so
