CC=gcc
CFLAGS= -Wall -std=c99 -pedantic -Wextra -Werror -fsanitize=address -g

OBJS=readelf.o
TARGET=readelf

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(TARGET) $(OBJS)

.PHONY:
	clean
