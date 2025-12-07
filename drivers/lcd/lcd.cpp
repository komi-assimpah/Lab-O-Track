#include "lcd.hpp"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

Lcd::Lcd(uint8_t address, uint8_t cols, uint8_t rows)
    : lcd_(nullptr), cols_(cols), rows_(rows) {
    // On alloue l'objet LiquidCrystal_I2C avec les paramètres fournis
    lcd_ = new LiquidCrystal_I2C(address, cols, rows);
}

Lcd::~Lcd() {
    if (lcd_) {
        delete lcd_;
        lcd_ = nullptr;
    }
}

void Lcd::init() {
    if (!lcd_) return;

    // Certaines variantes de la librairie utilisent init(), d'autres begin().
    // La plupart fournissent init() — on l'appelle ici.
    lcd_->init();
    lcd_->backlight();
    lcd_->clear();
}

void Lcd::clear() {
    if (!lcd_) return;
    lcd_->clear();
}

void Lcd::home() {
    if (!lcd_) return;
    lcd_->home();
}

void Lcd::setCursor(uint8_t col, uint8_t row) {
    if (!lcd_) return;
    lcd_->setCursor(col, row);
}

void Lcd::print(const char* str) {
    if (!lcd_ || !str) return;
    lcd_->print(str);
}

void Lcd::printAt(uint8_t col, uint8_t row, const char* str) {
    if (!lcd_ || !str) return;
    lcd_->setCursor(col, row);
    lcd_->print(str);
}

void Lcd::backlightOn() {
    if (!lcd_) return;
    lcd_->backlight();
}

void Lcd::backlightOff() {
    if (!lcd_) return;
    lcd_->noBacklight();
}

void Lcd::setBacklight(bool on) {
    if (!lcd_) return;
    if (on) lcd_->backlight(); else lcd_->noBacklight();
}

void Lcd::createChar(uint8_t index, uint8_t data[8]) {
    if (!lcd_ || index > 7 || !data) return;
    lcd_->createChar(index, data);
}


