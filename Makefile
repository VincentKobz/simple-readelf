CC=gcc
CFLAGS= -Wall -std=c99 -pedantic -Wextra -Werror -fsanitize=address -g -Iinclude

OBJS=$(patsubst %.c,%.o,$(wildcard src/*.c))
TARGET=simple-readelf

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(TARGET) $(OBJS)

.PHONY:
	clean
