/*
 * ============================================================================
 * TEST I2C DRIVER
 * ============================================================================
 * 
 * Ce fichier teste les fonctions du driver I2C.
 * 
 * Matériel requis :
 *   - Bus I2C fonctionnel (SDA=A4, SCL=A5)
 *   - Périphérique I2C connecté (LCD ou autre)
 *   - LEDs pour indication visuelle
 *   - Buzzer pour feedback sonore
 * 
 * Tests effectués :
 *   1. Initialisation I2C
 *   2. Scan du bus I2C (recherche d'adresses)
 *   3. Test de communication avec un périphérique
 *   4. Lecture du statut I2C
 */

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
    _delay_ms(1000);
    
    // Initialiser I2C
    i2c_init();
    
    led_on(LED_GREEN);
    buzzer_beep(200);
    _delay_ms(500);
    led_off(LED_GREEN);
    
    // ========================================================================
    // TEST 1 : Scan du bus I2C
    // ========================================================================
    
    // Recherche des périphériques I2C (adresses 0x00 à 0x7F)
    uint8_t devices_found = 0;
    
    led_on(LED_BLUE);
    
    for (uint8_t addr = 0x00; addr < 0x80; addr++) {
        // Envoyer START
        if (i2c_start()) {
            // Envoyer adresse en mode écriture
            if (i2c_write(addr << 1)) {
                // ACK reçu = périphérique trouvé !
                devices_found++;
                
                // Feedback visuel
                led_blink(LED_GREEN, 50);
                buzzer_beep(50);
                _delay_ms(100);
            }
            // Envoyer STOP
            i2c_stop();
        }
        _delay_ms(5);
    }
    
    led_off(LED_BLUE);
    
    // Résultat du scan
    if (devices_found > 0) {
        // Périphériques trouvés
        led_pattern_success();
        buzzer_pattern_success();
        
        // Clignotement = nombre de périphériques
        _delay_ms(1000);
        for (uint8_t i = 0; i < devices_found; i++) {
            led_blink(LED_GREEN, 200);
            buzzer_beep(100);
            _delay_ms(500);
        }
    } else {
        // Aucun périphérique trouvé
        led_pattern_alert();
        buzzer_pattern_error();
    }
    
    _delay_ms(2000);
    
    // ========================================================================
    // TEST 2 : Test de communication avec adresse 0x3E (LCD Grove)
    // ========================================================================
    
    led_on(LED_BUILTIN);
    
    bool success = false;
    
    if (i2c_start()) {
        if (i2c_write(0x3E << 1)) {
            // Communication réussie
            success = true;
        }
        i2c_stop();
    }
    
    led_off(LED_BUILTIN);
    
    if (success) {
        led_on(LED_GREEN);
        buzzer_pattern_success();
        _delay_ms(1000);
        led_off(LED_GREEN);
    } else {
        led_on(LED_RED);
        buzzer_pattern_error();
        _delay_ms(1000);
        led_off(LED_RED);
    }
    
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 3 : Test statut I2C
    // ========================================================================
    
    led_on(LED_BLUE);
    
    // Envoyer START et lire le statut
    if (i2c_start()) {
        uint8_t status = i2c_get_status();
        
        // Vérifier si le statut est correct
        if (status == I2C_STATUS_START || status == I2C_STATUS_REP_START) {
            led_blink(LED_GREEN, 100);
            buzzer_beep(100);
        }
        
        i2c_stop();
    }
    
    led_off(LED_BLUE);
    _delay_ms(1000);
    
    // ========================================================================
    // BOUCLE INFINIE : Scan périodique toutes les 5 secondes
    // ========================================================================
    
    while (1) {
        led_on(LED_BUILTIN);
        
        // Scan rapide
        uint8_t count = 0;
        for (uint8_t addr = 0x00; addr < 0x80; addr++) {
            if (i2c_start()) {
                if (i2c_write(addr << 1)) {
                    count++;
                }
                i2c_stop();
            }
        }
        
        led_off(LED_BUILTIN);
        
        // Afficher le résultat
        if (count > 0) {
            led_blink(LED_GREEN, 100);
            buzzer_beep(50);
        } else {
            led_blink(LED_RED, 100);
        }
        
        _delay_ms(5000);
    }
    
    return 0;
}
