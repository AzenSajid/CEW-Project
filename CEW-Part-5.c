CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -I/usr/include/cjson
LDFLAGS = -lcurl -lcjson
TARGET = weather_app
SRC = main.c functions.c
HEADERS = functions.h
OBJECTS = $(SRC:.c=.o)

.PHONY: all clean

# Default target
all: $(TARGET)

# Link the final executable
$(TARGET): $(OBJECTS)
    $(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files to object files
%.o: %.c $(HEADERS)
    $(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
    rm -f $(OBJECTS) $(TARGET)
