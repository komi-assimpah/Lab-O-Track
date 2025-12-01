# Configuration
MCU = atmega328p
F_CPU = 16000000UL
PROGRAMMER = arduino
PORT = /dev/ttyACM0

# Outils
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# Flags
CFLAGS = -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Wall -Wextra -I.
LDFLAGS = -mmcu=$(MCU)

# Fichiers
TARGET = main
SRC = main.c drivers/led/led.c drivers/buzzer/buzzer.c
OBJ = $(SRC:.c=.o)

# Compilation
all: $(TARGET).hex

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	avr-size --format=avr --mcu=$(MCU) $(TARGET).elf

# Upload
upload: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -U flash:w:$<:i

# Nettoyage
clean:
	rm -f $(TARGET).elf $(TARGET).hex $(OBJ)

# Monitor
monitor:
	screen $(PORT) 9600

.PHONY: all upload clean monitor