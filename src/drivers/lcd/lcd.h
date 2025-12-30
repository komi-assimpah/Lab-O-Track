#ifndef LCD_H
#define LCD_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * ============================================================================
 * DRIVER LCD GROVE 16x2 (Black on Yellow) v2.0
 * ============================================================================
 *
 * Ce driver contrôle l'écran LCD Grove 16x2 via I2C.
 * L'écran utilise le contrôleur HD44780 compatible.
 *
 * Caractéristiques :
 *   - 16 colonnes x 2 lignes
 *   - Communication I2C
 *   - Adresse I2C par défaut : 0x3E
 *   - Backlight RGB (adresse 0x62) - non géré dans cette version simple
 */

#define LCD_I2C_ADDR 0x3E
#define LCD_COLS 16
#define LCD_ROWS 2

// ============================================================================
// COMMANDES HD44780 (pour référence)
// ============================================================================

#define LCD_CMD_CLEAR 0x01          // Efface l'écran
#define LCD_CMD_HOME 0x02           // Retour à la position (0,0)
#define LCD_CMD_ENTRY_MODE 0x04     // Mode d'entrée
#define LCD_CMD_DISPLAY_CTRL 0x08   // Contrôle affichage
#define LCD_CMD_SHIFT 0x10          // Décalage curseur/écran
#define LCD_CMD_FUNCTION_SET 0x20   // Configuration fonction
#define LCD_CMD_SET_CGRAM_ADDR 0x40 // Adresse CGRAM (caractères custom)
#define LCD_CMD_SET_DDRAM_ADDR 0x80 // Adresse DDRAM (position curseur)

// Flags pour Entry Mode (0x04)
#define LCD_ENTRY_RIGHT 0x00     // Curseur vers la droite
#define LCD_ENTRY_LEFT 0x02      // Curseur vers la gauche
#define LCD_ENTRY_SHIFT_INC 0x01 // Décalage auto de l'écran
#define LCD_ENTRY_SHIFT_DEC 0x00 // Pas de décalage

// Flags pour Display Control (0x08)
#define LCD_DISPLAY_ON 0x04  // Affichage ON
#define LCD_DISPLAY_OFF 0x00 // Affichage OFF
#define LCD_CURSOR_ON 0x02   // Curseur visible
#define LCD_CURSOR_OFF 0x00  // Curseur invisible
#define LCD_BLINK_ON 0x01    // Curseur clignotant
#define LCD_BLINK_OFF 0x00   // Curseur fixe

#define LCD_8BIT_MODE 0x10 // Mode 8 bits
#define LCD_4BIT_MODE 0x00 // Mode 4 bits
#define LCD_2LINE 0x08     // 2 lignes
#define LCD_1LINE 0x00     // 1 ligne
#define LCD_5x10_DOTS 0x04 // Caractères 5x10
#define LCD_5x8_DOTS 0x00  // Caractères 5x8


/**
 * Initialise l'écran LCD
 *
 * Configure l'écran en mode 2 lignes, curseur invisible, affichage ON.
 * IMPORTANT : Appeler i2c_init() AVANT d'appeler lcd_init() !
 *
 * Retourne : true si succès, false si erreur
 */
bool lcd_init(void);


/**
 * Efface tout l'écran et ramène le curseur en position (0,0)
 */
void lcd_clear(void);


/**
 * Ramène le curseur en position (0,0) sans effacer l'écran
 */
void lcd_home(void);


/*
  * Positionne le curseur à une ligne et colonne donnée
  *
  * Notre LCD a une organisation mémoire particulière :
  * - Ligne 0 : adresses 0x00 à 0x0F (colonnes 0-15)
  * - Ligne 1 : adresses 0x40 à 0x4F (colonnes 0-15)
  */
void lcd_set_cursor(uint8_t row, uint8_t col);


/**
 * Affiche un caractère à la position actuelle du curseur
 *
 * Paramètres :
 *   - c : le caractère à afficher
 */
void lcd_print_char(char c);


/**
 * Affiche une chaîne de caractères à la position actuelle du curseur
 *
 * Paramètres :
 *   - str : la chaîne à afficher (terminée par '\0')
 */
void lcd_print(const char *str);


/**
 * Active ou désactive l'affichage
 *
 * Paramètres :
 *   - on : true pour activer, false pour désactiver
 */
void lcd_display(bool on);


/**
 * Active ou désactive le curseur
 *
 * Paramètres :
 *   - on : true pour afficher le curseur, false pour le cacher
 */
void lcd_cursor(bool on);


/**
 * Active ou désactive le clignotement du curseur
 *
 * Paramètres :
 *   - on : true pour faire clignoter, false pour curseur fixe
 */
void lcd_blink(bool on);

#ifdef __cplusplus
}
#endif

#endif
