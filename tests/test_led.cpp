/*
 * ============================================================================
 * TEST LED DRIVER
 * ============================================================================
 * 
 * Ce fichier teste toutes les fonctions du driver LED.
 * 
 * Matériel requis :
 *   - 3 LEDs externes connectées sur D2 (Rouge), D3 (Verte), D4 (Bleue)
 *   - LED intégrée sur Pin 13
 * 
 * Tests effectués :
 *   1. Allumage/extinction individuel de chaque LED
 *   2. Toggle des LEDs
 *   3. Blink avec délai
 *   4. Contrôle groupé (toutes les LEDs)
 *   5. Patterns prédéfinis
 */

#include "../drivers/led/led.h"
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // ========================================================================
    // INITIALISATION
    // ========================================================================
    led_init_all();
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 1 : Allumage/extinction individuel
    // ========================================================================
    
    // Test LED Rouge
    led_on(LED_RED);
    _delay_ms(1000);
    led_off(LED_RED);
    _delay_ms(500);
    
    // Test LED Verte
    led_on(LED_GREEN);
    _delay_ms(1000);
    led_off(LED_GREEN);
    _delay_ms(500);
    
    // Test LED Bleue
    led_on(LED_BLUE);
    _delay_ms(1000);
    led_off(LED_BLUE);
    _delay_ms(500);
    
    // Test LED intégrée
    led_on(LED_BUILTIN);
    _delay_ms(1000);
    led_off(LED_BUILTIN);
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 2 : Toggle
    // ========================================================================
    
    for (uint8_t i = 0; i < 5; i++) {
        led_toggle(LED_RED);
        _delay_ms(300);
    }
    led_off(LED_RED);
    _delay_ms(500);
    
    // ========================================================================
    // TEST 3 : Blink
    // ========================================================================
    
    for (uint8_t i = 0; i < 3; i++) {
        led_blink(LED_GREEN, 200);
        _delay_ms(300);
    }
    _delay_ms(500);
    
    // ========================================================================
    // TEST 4 : Contrôle groupé
    // ========================================================================
    
    led_all_on();
    _delay_ms(2000);
    led_all_off();
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 5 : Patterns prédéfinis
    // ========================================================================
    
    // Pattern de démarrage
    led_pattern_startup();
    _delay_ms(1000);
    
    // Pattern de succès
    led_pattern_success();
    _delay_ms(1000);
    
    // Pattern d'alerte
    led_pattern_alert();
    _delay_ms(1000);
    
    // Pattern séquence
    led_pattern_sequence();
    _delay_ms(1000);
    
    // ========================================================================
    // BOUCLE INFINIE : Clignotement LED verte pour indiquer test terminé
    // ========================================================================
    
    while (1) {
        led_blink(LED_GREEN, 100);
        _delay_ms(2000);
    }
    
    return 0;
}
