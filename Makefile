CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -pthread
LDFLAGS = -lmosquitto -pthread

SRC = src/main.c src/pac1944.c src/mqtt_client.c src/led_ctrl.c src/app_config.c src/logger.c
OBJ = $(SRC:.c=.o)

TARGET = power-gateway

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install:
	mkdir -p $(DESTDIR)/usr/bin
	cp $(TARGET) $(DESTDIR)/usr/bin/

clean:
	rm -f $(OBJ) $(TARGET)