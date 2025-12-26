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

# Chemins des librairies
ARDUINO_CORE = lib/arduinoLibsAndCore/cores/arduino
ARDUINO_VARIANTS = lib/arduinoLibsAndCore/variants/standard
FREERTOS_INC = lib/FreeRTOS-Kernel/include
FREERTOS_PORT = lib/FreeRTOS-Kernel/portable/GCC/ATMega328
SOFTSERIAL = lib/arduinoLibsAndCore/libraries/SoftwareSerial/src
ARDUINO_LIB = lib/arduinoLibsAndCore/libres.a

# Flags avec tous les includes
INCLUDES = -I. -I$(ARDUINO_CORE) -I$(ARDUINO_VARIANTS) -I$(FREERTOS_INC) -I$(FREERTOS_PORT) -I$(SOFTSERIAL)
CXXFLAGS = -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Wall -Wextra $(INCLUDES) -fno-exceptions -fno-rtti -fno-lto
CFLAGS = -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Wall -Wextra $(INCLUDES) -fno-lto
LDFLAGS = -mmcu=$(MCU) -fno-lto

# Fichiers
TARGET = main

# Sources C++ (application + drivers)
CPP_SRC = main.cpp drivers/led/led.cpp drivers/buzzer/buzzer.cpp drivers/i2c/i2c.cpp drivers/lcd/lcd.cpp drivers/button/button.cpp drivers/rfid/rfid.cpp lib/arduinoLibsAndCore/libraries/SoftwareSerial/src/SoftwareSerial.cpp
CPP_OBJ = $(CPP_SRC:.cpp=.o)

# Sources C (FreeRTOS Kernel)
FREERTOS_SRC = lib/FreeRTOS-Kernel/tasks.c \
               lib/FreeRTOS-Kernel/queue.c \
               lib/FreeRTOS-Kernel/list.c \
               lib/FreeRTOS-Kernel/timers.c \
               lib/FreeRTOS-Kernel/portable/GCC/ATMega328/port.c \
               lib/FreeRTOS-Kernel/portable/MemMang/heap_1.c
FREERTOS_OBJ = $(FREERTOS_SRC:.c=.o)

# Compilation
all: $(TARGET).hex

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(CPP_OBJ) $(FREERTOS_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(ARDUINO_LIB)

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