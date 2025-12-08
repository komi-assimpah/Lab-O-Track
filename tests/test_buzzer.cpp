/*
 * ============================================================================
 * TEST BUZZER DRIVER
 * ============================================================================
 * 
 * Ce fichier teste toutes les fonctions du driver Buzzer.
 * 
 * Matériel requis :
 *   - Buzzer connecté sur D6
 * 
 * Tests effectués :
 *   1. Buzzer ON/OFF basique
 *   2. Beep simple avec durée
 *   3. Beep bloquant
 *   4. Patterns sonores prédéfinis
 *   5. Patterns complexes (siren, morse SOS)
 */

#include "../drivers/buzzer/buzzer.h"
#include "../drivers/led/led.h"
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // ========================================================================
    // INITIALISATION
    // ========================================================================
    buzzer_init();
    led_init_all();
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 1 : ON/OFF basique
    // ========================================================================
    
    led_on(LED_RED);
    buzzer_on();
    _delay_ms(500);
    buzzer_off();
    led_off(LED_RED);
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 2 : Beep simple
    // ========================================================================
    
    led_on(LED_GREEN);
    for (uint8_t i = 0; i < 3; i++) {
        buzzer_beep(100);
        _delay_ms(300);
    }
    led_off(LED_GREEN);
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 3 : Beep bloquant
    // ========================================================================
    
    led_on(LED_BLUE);
    buzzer_beep_blocking(500);
    led_off(LED_BLUE);
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 4 : Pattern startup
    // ========================================================================
    
    led_pattern_startup();
    buzzer_pattern_startup();
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 5 : Pattern success
    // ========================================================================
    
    led_pattern_success();
    buzzer_pattern_success();
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 6 : Pattern warning
    // ========================================================================
    
    led_on(LED_RED);
    buzzer_pattern_warning();
    led_off(LED_RED);
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 7 : Pattern error
    // ========================================================================
    
    led_on(LED_RED);
    buzzer_pattern_error();
    led_off(LED_RED);
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 8 : Pattern alert
    // ========================================================================
    
    led_pattern_alert();
    buzzer_pattern_alert();
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 9 : Siren
    // ========================================================================
    
    led_all_on();
    buzzer_pattern_siren(3);
    led_all_off();
    _delay_ms(2000);
    
    // ========================================================================
    // TEST 10 : Morse SOS
    // ========================================================================
    
    led_on(LED_BUILTIN);
    buzzer_pattern_morse_sos();
    led_off(LED_BUILTIN);
    _delay_ms(2000);
    
    // ========================================================================
    // BOUCLE INFINIE : Bip toutes les 3 secondes
    // ========================================================================
    
    while (1) {
        led_blink(LED_GREEN, 50);
        buzzer_beep(50);
        _delay_ms(3000);
    }
    
    return 0;
}
