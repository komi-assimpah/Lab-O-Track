#include "lcd.h"
#include "../i2c/i2c.h"
#include <string.h>
#include <util/delay.h>


// Registres de contrôle pour l'envoi de commandes/données
#define LCD_REG_COMMAND 0x80 // Co=1, RS=0 : Commande
#define LCD_REG_DATA 0x40    // Co=0, RS=1 : Donnée

// État actuel de l'affichage (pour les fonctions display/cursor/blink)
static uint8_t lcd_display_control = LCD_CMD_DISPLAY_CTRL | LCD_DISPLAY_ON;


/**
 * Envoie une commande au LCD
 *
 * Paramètres :
 *   - cmd : le byte de commande à envoyer
 */
static void lcd_send_command(uint8_t cmd) {
  i2c_start();
  i2c_write(LCD_I2C_ADDR << 1); // Adresse + bit W (0)
  i2c_write(LCD_REG_COMMAND);   // Mode commande
  i2c_write(cmd);               // La commande
  i2c_stop();

  if (cmd == LCD_CMD_CLEAR || cmd == LCD_CMD_HOME) {
    _delay_ms(2);
  } else {
    _delay_us(50);
  }
}


/**
 * Envoie une donnée (caractère) au LCD
 *
 * Paramètres :
 *   - data : le byte de donnée à envoyer
 */
static void lcd_send_data(uint8_t data) {
  i2c_start();
  i2c_write(LCD_I2C_ADDR << 1);
  i2c_write(LCD_REG_DATA);
  i2c_write(data);
  i2c_stop();

  _delay_us(50);
}



bool lcd_init(void) {
  _delay_ms(50);

  lcd_send_command(LCD_CMD_FUNCTION_SET | LCD_8BIT_MODE | LCD_2LINE |
                   LCD_5x8_DOTS);
  _delay_ms(5);

  lcd_display_control = LCD_CMD_DISPLAY_CTRL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF;
  lcd_send_command(lcd_display_control);


  lcd_clear();

  /*
   * Mode d'entrée :
   * - Curseur se déplace vers la droite après chaque caractère
   * - Pas de décalage automatique de l'écran
   */
  lcd_send_command(LCD_CMD_ENTRY_MODE | LCD_ENTRY_RIGHT | LCD_ENTRY_SHIFT_DEC);

  _delay_ms(5);

  return true;
}

void lcd_clear(void) {
  /*
   * Envoie la commande CLEAR
   * Efface tout l'écran et ramène le curseur en (0,0)
   */
  lcd_send_command(LCD_CMD_CLEAR);
  _delay_ms(2); // CLEAR prend du temps !
}

void lcd_home(void) {
  /*
   * Ramène le curseur en position (0,0)
   * sans effacer l'écran
   */
  lcd_send_command(LCD_CMD_HOME);
  _delay_ms(2); // HOME prend aussi du temps
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
  /*
   * Positionne le curseur à une ligne et colonne donnée
   *
   * Le LCD a une organisation mémoire particulière :
   * - Ligne 0 : adresses 0x00 à 0x0F (colonnes 0-15)
   * - Ligne 1 : adresses 0x40 à 0x4F (colonnes 0-15)
   */

  // Limite les valeurs pour éviter les erreurs
  if (row >= LCD_ROWS)
    row = LCD_ROWS - 1;
  if (col >= LCD_COLS)
    col = LCD_COLS - 1;

  /*
   * Calcul de l'adresse DDRAM
   * Ligne 0 commence à 0x00
   * Ligne 1 commence à 0x40
   */
  uint8_t addr = col;
  if (row == 1) {
    addr += 0x40;
  }

  /*
   * Envoie la commande SET_DDRAM_ADDR avec l'adresse
   */
  lcd_send_command(LCD_CMD_SET_DDRAM_ADDR | addr);
}

void lcd_print_char(char c) {
  /*
   * Affiche un seul caractère
   */
  lcd_send_data((uint8_t)c);
}

void lcd_print(const char *str) {
  /*
   * Affiche une chaîne de caractères
   * Parcourt la chaîne jusqu'au '\0'
   */
  while (*str) {
    lcd_print_char(*str);
    str++;
  }
}

void lcd_display(bool on) {
  /*
   * Active ou désactive l'affichage
   * (le contenu reste en mémoire)
   */
  if (on) {
    lcd_display_control |= LCD_DISPLAY_ON;
  } else {
    lcd_display_control &= ~LCD_DISPLAY_ON;
  }
  lcd_send_command(lcd_display_control);
}

void lcd_cursor(bool on) {
  /*
   * Affiche ou cache le curseur
   */
  if (on) {
    lcd_display_control |= LCD_CURSOR_ON;
  } else {
    lcd_display_control &= ~LCD_CURSOR_ON;
  }
  lcd_send_command(lcd_display_control);
}

void lcd_blink(bool on) {
  /*
   * Active ou désactive le clignotement du curseur
   */
  if (on) {
    lcd_display_control |= LCD_BLINK_ON;
  } else {
    lcd_display_control &= ~LCD_BLINK_ON;
  }
  lcd_send_command(lcd_display_control);
}
