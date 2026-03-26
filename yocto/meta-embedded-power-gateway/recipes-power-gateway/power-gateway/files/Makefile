CC ?= gcc
CFLAGS ?= -Wall -Wextra -I. -pthread
LDLIBS += -lmosquitto -pthread

SRC = main.c app_config.c led_ctrl.c mqtt_client.c pac1944.c logger.c
OBJ = $(SRC:.c=.o)

TARGET = power-gateway

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
