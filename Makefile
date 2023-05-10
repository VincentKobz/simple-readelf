CC=gcc
CFLAGS= -Wall -std=c99 -pedantic -Wextra -Werror -Iinclude
CFLAGS_DEBUG= -fsanitize=address -g

OBJS=$(patsubst %.c,%.o,$(wildcard src/*.c))
TARGET=simple-readelf

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

debug: $(OBJS)
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) -o $(TARGET) $^

clean:
	$(RM) $(TARGET) $(OBJS)

.PHONY:
	clean
