# Configuration
MCU = atmega328p
F_CPU = 16000000UL
PROGRAMMER = arduino
PORT = /dev/ttyACM0

# Outils
CXX = avr-g++
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

# Flags
CXXFLAGS = -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Wall -Wextra -I. -fno-exceptions -fno-rtti
CFLAGS = -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Wall -Wextra -I.
LDFLAGS = -mmcu=$(MCU)

# Fichiers
TARGET = main
CPP_SRC = main.cpp drivers/led/led.cpp drivers/buzzer/buzzer.cpp drivers/i2c/i2c.cpp drivers/lcd/lcd.cpp drivers/button/button.cpp
CPP_OBJ = $(CPP_SRC:.cpp=.o)

# Compilation
all: $(TARGET).hex

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(CPP_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	avr-size --format=avr --mcu=$(MCU) $(TARGET).elf

# Upload
upload: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -U flash:w:$<:i

# Nettoyage
clean:
	rm -f $(TARGET).elf $(TARGET).hex $(CPP_OBJ)
	rm -f tests/*.elf tests/*.hex tests/*.o

# Nettoyage complet (inclut les fichiers objets des drivers)
clean-all: clean
	find drivers -name "*.o" -delete

# Monitor
monitor:
	screen $(PORT) 9600

.PHONY: all upload clean monitor