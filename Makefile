CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -pthread
LDFLAGS = -lmosquitto -pthread

SRC = src/main.c src/app_config.c src/led_ctrl.c src/mqtt_client.c src/pac1944.c src/logger.c
OBJ = $(SRC:.c=.o)

TARGET = power-gateway

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
run: all
	sudo ./power-gateway

clean:
	rm -f $(OBJ) $(TARGET)