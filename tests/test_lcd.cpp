/*
 * ============================================================================
 * TEST LCD DRIVER
 * ============================================================================
 * 
 * Ce fichier teste toutes les fonctions du driver LCD Grove 16x2.
 * 
 * Matériel requis :
 *   - LCD Grove 16x2 connecté en I2C (adresse 0x3E)
 *   - Bus I2C fonctionnel (SDA=A4, SCL=A5)
 *   - LEDs pour indication visuelle
 *   - Buzzer pour feedback sonore
 * 
 * Tests effectués :
 *   1. Initialisation LCD
 *   2. Affichage de texte simple
 *   3. Positionnement du curseur
 *   4. Clear et Home
 *   5. Affichage de caractères individuels
 *   6. Compteur dynamique
 *   7. Messages défilants
 * 
 * IMPORTANT : Ce test nécessite que le LCD soit connecté !
 */

#include "../drivers/lcd/lcd.h"
#include "../drivers/i2c/i2c.h"
#include "../drivers/led/led.h"
#include "../drivers/buzzer/buzzer.h"
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // ========================================================================
    // INITIALISATION
    // ========================================================================
    led_init_all();
    buzzer_init();
    
    led_pattern_startup();
    buzzer_pattern_startup();
    _delay_ms(500);
    
    // Initialiser I2C AVANT LCD
    i2c_init();
    _delay_ms(100);
    
    // Initialiser LCD
    led_on(LED_BLUE);
    bool lcd_ok = lcd_init();
    led_off(LED_BLUE);
    
    if (!lcd_ok) {
        // Erreur d'initialisation
        led_pattern_alert();
        buzzer_pattern_error();
        
        // Boucle infinie d'erreur
        while (1) {
            led_blink(LED_RED, 200);
            _delay_ms(1000);
        }
    }
    
    // Initialisation réussie
    led_on(LED_GREEN);
    buzzer_pattern_success();
    _delay_ms(500);
    led_off(LED_GREEN);
    
    // ========================================================================
    // TEST 1 : Affichage simple
    // ========================================================================
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Test LCD");
    lcd_set_cursor(1, 0);
    lcd_print("Driver OK!");
    
    led_blink(LED_GREEN, 100);
    buzzer_beep(100);
    _delay_ms(2000);
    
    // ========================================================================
    // TEST 2 : Clear et repositionnement
    // ========================================================================
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Clear Test");
    _delay_ms(1500);
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Cleared!");
    
    led_blink(LED_BLUE, 100);
    _delay_ms(1500);
    
    // ========================================================================
    // TEST 3 : Positionnement précis
    // ========================================================================
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Position Test");
    
    // Afficher des caractères à des positions spécifiques
    lcd_set_cursor(1, 0);
    lcd_print_char('A');
    
    lcd_set_cursor(1, 5);
    lcd_print_char('B');
    
    lcd_set_cursor(1, 10);
    lcd_print_char('C');
    
    lcd_set_cursor(1, 15);
    lcd_print_char('D');
    
    _delay_ms(2000);
    
    // ========================================================================
    // TEST 4 : Compteur 0-99
    // ========================================================================
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Compteur:");
    
    for (uint8_t i = 0; i < 100; i++) {
        lcd_set_cursor(1, 0);
        lcd_print("Valeur: ");
        
        // Afficher le nombre (dizaines et unités)
        if (i >= 10) {
            lcd_print_char('0' + (i / 10));
        } else {
            lcd_print_char(' ');
        }
        lcd_print_char('0' + (i % 10));
        
        led_toggle(LED_GREEN);
        
        if (i % 10 == 0) {
            buzzer_beep(30);
        }
        
        _delay_ms(100);
    }
    
    led_off(LED_GREEN);
    buzzer_pattern_success();
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 5 : Messages alternés
    // ========================================================================
    
    for (uint8_t i = 0; i < 5; i++) {
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("Lab-O-Track");
        lcd_set_cursor(1, 0);
        lcd_print("System Ready");
        
        led_on(LED_BLUE);
        _delay_ms(1500);
        led_off(LED_BLUE);
        
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("RFID Tracking");
        lcd_set_cursor(1, 0);
        lcd_print("Active!");
        
        led_on(LED_GREEN);
        _delay_ms(1500);
        led_off(LED_GREEN);
    }
    
    // ========================================================================
    // TEST 6 : Simulation alerte
    // ========================================================================
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("ALERT!");
    lcd_set_cursor(1, 0);
    lcd_print("OSC-01 MISSING");
    
    for (uint8_t i = 0; i < 5; i++) {
        led_on(LED_RED);
        buzzer_beep(100);
        _delay_ms(200);
        led_off(LED_RED);
        _delay_ms(200);
    }
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Equipment");
    lcd_set_cursor(1, 0);
    lcd_print("Returned!");
    
    led_pattern_success();
    buzzer_pattern_success();
    _delay_ms(2000);
    
    // ========================================================================
    // BOUCLE INFINIE : Affichage dynamique
    // ========================================================================
    
    uint16_t seconds = 0;
    
    while (1) {
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("Uptime:");
        
        lcd_set_cursor(1, 0);
        
        // Afficher secondes (max 9999)
        uint16_t remaining = seconds;
        
        if (seconds >= 1000) {
            lcd_print_char('0' + (remaining / 1000));
            remaining %= 1000;
        }
        if (seconds >= 100) {
            lcd_print_char('0' + (remaining / 100));
            remaining %= 100;
        }
        if (seconds >= 10) {
            lcd_print_char('0' + (remaining / 10));
            remaining %= 10;
        }
        lcd_print_char('0' + remaining);
        
        lcd_print(" sec");
        
        led_blink(LED_BUILTIN, 50);
        
        _delay_ms(1000);
        seconds++;
        
        // Bip toutes les 10 secondes
        if (seconds % 10 == 0) {
            buzzer_beep(50);
        }
    }
    
    return 0;
}
