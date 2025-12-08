# ============================================================================
# MAKEFILE POUR LES TESTS DES DRIVERS
# ============================================================================
# 
# Ce Makefile permet de compiler et uploader individuellement chaque test.
# 
# Utilisation :
#   make test_led       - Compile et upload le test LED
#   make test_buzzer    - Compile et upload le test Buzzer
#   make test_button    - Compile et upload le test Button
#   make test_i2c       - Compile et upload le test I2C
#   make test_lcd       - Compile et upload le test LCD
#   make all_tests      - Compile tous les tests
#   make clean          - Nettoie les fichiers compilés

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
CXXFLAGS = -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Wall -Wextra -I.. -fno-exceptions -fno-rtti
LDFLAGS = -mmcu=$(MCU)

# Drivers communs
DRIVER_DIR = ../drivers
LED_SRC = $(DRIVER_DIR)/led/led.cpp
BUZZER_SRC = $(DRIVER_DIR)/buzzer/buzzer.cpp
BUTTON_SRC = $(DRIVER_DIR)/button/button.cpp
I2C_SRC = $(DRIVER_DIR)/i2c/i2c.cpp
LCD_SRC = $(DRIVER_DIR)/lcd/lcd.cpp
RFID_SRC = $(DRIVER_DIR)/rfid/rfid.cpp

LED_OBJ = $(LED_SRC:.cpp=.o)
BUZZER_OBJ = $(BUZZER_SRC:.cpp=.o)
BUTTON_OBJ = $(BUTTON_SRC:.cpp=.o)
I2C_OBJ = $(I2C_SRC:.cpp=.o)
LCD_OBJ = $(LCD_SRC:.cpp=.o)
RFID_OBJ = $(RFID_SRC:.cpp=.o)

# ============================================================================
# RÈGLES DE COMPILATION
# ============================================================================

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.elf: %.o
	$(CXX) $(LDFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	avr-size --format=avr --mcu=$(MCU) $<

# ============================================================================
# TESTS INDIVIDUELS
# ============================================================================

# Test LED
test_led.o: test_led.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test_led.elf: test_led.o $(LED_OBJ) $(BUZZER_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test_led: test_led.hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -U flash:w:$<:i

# Test Buzzer
test_buzzer.o: test_buzzer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test_buzzer.elf: test_buzzer.o $(BUZZER_OBJ) $(LED_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test_buzzer: test_buzzer.hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -U flash:w:$<:i

# Test Button
test_button.o: test_button.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test_button.elf: test_button.o $(BUTTON_OBJ) $(LED_OBJ) $(BUZZER_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test_button: test_button.hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -U flash:w:$<:i

# Test I2C
test_i2c.o: test_i2c.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test_i2c.elf: test_i2c.o $(I2C_OBJ) $(LED_OBJ) $(BUZZER_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test_i2c: test_i2c.hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -U flash:w:$<:i

# Test LCD
test_lcd.o: test_lcd.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test_lcd.elf: test_lcd.o $(LCD_OBJ) $(I2C_OBJ) $(LED_OBJ) $(BUZZER_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test_lcd: test_lcd.hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -U flash:w:$<:i

# Test RFID
test_rfid.o: test_rfid.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test_rfid.elf: test_rfid.o $(RFID_OBJ) $(LCD_OBJ) $(I2C_OBJ) $(LED_OBJ) $(BUZZER_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

test_rfid: test_rfid.hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -U flash:w:$<:i

# ============================================================================
# RÈGLES GÉNÉRALES
# ============================================================================

# Compiler tous les tests (sans upload)
all_tests: test_led.hex test_buzzer.hex test_button.hex test_i2c.hex test_lcd.hex test_rfid.hex
	@echo "====================================="
	@echo "Tous les tests ont été compilés !"
	@echo "====================================="

# Nettoyage
clean:
	rm -f *.o *.elf *.hex
	rm -f $(DRIVER_DIR)/*/*.o

# Afficher les infos
info:
	@echo "Tests disponibles :"
	@echo "  make test_led     - Test du driver LED"
	@echo "  make test_buzzer  - Test du driver Buzzer"
	@echo "  make test_button  - Test du driver Button"
	@echo "  make test_i2c     - Test du driver I2C"
	@echo "  make test_lcd     - Test du driver LCD"
	@echo "  make test_rfid    - Test du driver RFID"
	@echo ""
	@echo "Commandes utiles :"
	@echo "  make all_tests    - Compiler tous les tests"
	@echo "  make clean        - Nettoyer les fichiers"
	@echo "  make info         - Afficher cette aide"

.PHONY: test_led test_buzzer test_button test_i2c test_lcd test_rfid all_tests clean info
