#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <LiquidCrystal_I2C.h>

/**
 * @brief Driver LCD I2C simple basé sur la librairie LiquidCrystal_I2C
 *
 * Fournit une API minimale : initialisation, effacement, affichage,
 * positionnement du curseur et contrôle du rétroéclairage.
 */
class Lcd {
public:
    /**
     * @brief Constructeur
     * @param address Adresse I2C de l'écran (par défaut 0x27)
     * @param cols Nombre de colonnes (par défaut 16)
     * @param rows Nombre de lignes (par défaut 2)
     */
    Lcd(uint8_t address = 0x27, uint8_t cols = 16, uint8_t rows = 2);

    ~Lcd();

    /** Initialise l'écran (doit être appelé depuis setup()) */
    void init();

    /** Efface l'écran */
    void clear();

    /** Replace le curseur à la position home (0,0) */
    void home();

    /** Place le curseur (col, row) */
    void setCursor(uint8_t col, uint8_t row);

    /** Affiche une chaîne C-style */
    void print(const char* str);

    /** Affiche une chaîne à la position donnée */
    void printAt(uint8_t col, uint8_t row, const char* str);

    /** Active le rétroéclairage */
    void backlightOn();

    /** Désactive le rétroéclairage */
    void backlightOff();

    /** Active/désactive le rétroéclairage */
    void setBacklight(bool on);

    /** Crée un caractère personnalisé (tableau de 8 octets) */
    void createChar(uint8_t index, uint8_t data[8]);

private:
    LiquidCrystal_I2C* lcd_; // alloué dynamiquement pour compatibilité
    uint8_t cols_;
    uint8_t rows_;
};

#endif // LCD_H
