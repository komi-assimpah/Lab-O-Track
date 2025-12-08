#include "drivers/buzzer/buzzer.h"
#include "drivers/i2c/i2c.h"
#include "drivers/lcd/lcd.h"
#include "drivers/led/led.h"
#include <avr/io.h>
#include <util/delay.h>


/*
 * ============================================================================
 * EXEMPLE D'UTILISATION DU LCD GROVE 16x2
 * ============================================================================
 *
 * Ce programme montre comment utiliser le driver LCD.
 * Il affiche différents messages sur l'écran.
 */

int main(void) {
  // ========================================================================
  // INITIALISATION
  // ========================================================================

  // Initialise les LEDs et le buzzer
  led_init_all();
  buzzer_init();

  // Signal de démarrage
  led_on(LED_GREEN);
  buzzer_beep(100);
  _delay_ms(200);
  led_off(LED_GREEN);

  // Initialise le bus I2C (IMPORTANT : à faire AVANT lcd_init !)
  i2c_init();

  // Initialise l'écran LCD
  lcd_init();

  // ========================================================================
  // AFFICHAGE SUR LE LCD
  // ========================================================================

  // Efface l'écran
  lcd_clear();

  // Affiche "Hello World!" sur la première ligne
  lcd_set_cursor(0, 0); // Ligne 0, colonne 0
  lcd_print("Hello World!");

  // Affiche "LCD Grove v2.0" sur la deuxième ligne
  lcd_set_cursor(1, 0); // Ligne 1, colonne 0
  lcd_print("LCD Grove v2.0");

  // Signal de succès
  led_on(LED_GREEN);
  buzzer_pattern_success();
  led_off(LED_GREEN);

  _delay_ms(3000);

  // ========================================================================
  // BOUCLE PRINCIPALE
  // ========================================================================

  while (1) {
    // Exemple 1 : Compteur
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Compteur:");

    for (uint8_t i = 0; i < 10; i++) {
      lcd_set_cursor(1, 0);
      lcd_print("Valeur: ");
      lcd_print_char('0' + i); // Affiche le chiffre

      led_on(LED_BLUE);
      buzzer_beep(50);
      led_off(LED_BLUE);

      _delay_ms(1000);
    }

    _delay_ms(1000);

    // Exemple 2 : Messages alternés
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Lab-O-Track");
    lcd_set_cursor(1, 0);
    lcd_print("Systeme actif");

    led_on(LED_GREEN);
    _delay_ms(2000);
    led_off(LED_GREEN);

    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Pret pour");
    lcd_set_cursor(1, 0);
    lcd_print("tracabilite!");

    led_on(LED_BLUE);
    _delay_ms(2000);
    led_off(LED_BLUE);

    // Exemple 3 : Test du curseur
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Test curseur");

    lcd_set_cursor(1, 0);
    lcd_cursor(true); // Active le curseur
    lcd_print("Visible");
    _delay_ms(2000);

    lcd_cursor(false); // Cache le curseur
    lcd_blink(true);   // Active le clignotement
    _delay_ms(2000);

    lcd_blink(false); // Désactive le clignotement

    _delay_ms(1000);
  }

  return 0;
}