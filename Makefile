CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
TARGET = c_squad
TESTER = tester

.PHONY: all test clean

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

$(TESTER): tester.c
	$(CC) $(CFLAGS) -D_POSIX_C_SOURCE=200809L tester.c -o $(TESTER)

test: $(TARGET) $(TESTER)
	./$(TESTER)

clean:
	rm -f $(TARGET) $(TESTER)
	rm -rf tester_outputs
