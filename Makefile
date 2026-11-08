CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -O2 -g -fsanitize=address,undefined
LDFLAGS = -lm

TARGET = csvstat
OBJS = main.o table.o csv_parser.o stats.o output.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)