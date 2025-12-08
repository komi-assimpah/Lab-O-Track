/*
 * ============================================================================
 * TEST BUTTON DRIVER
 * ============================================================================
 * 
 * Ce fichier teste toutes les fonctions du driver Button.
 * 
 * Matériel requis :
 *   - Bouton connecté sur D7 (avec pull-up interne activé)
 *   - LEDs pour indication visuelle
 *   - Buzzer pour feedback sonore
 * 
 * Tests effectués :
 *   1. Détection d'appui simple (polling)
 *   2. Attente d'appui avec LED indicatrice
 *   3. Attente de relâchement
 *   4. Compteur d'appuis avec affichage LED
 */

#include "../drivers/button/button.h"
#include "../drivers/led/led.h"
#include "../drivers/buzzer/buzzer.h"
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // ========================================================================
    // INITIALISATION
    // ========================================================================
    button_init();
    led_init_all();
    buzzer_init();
    
    // Signal de démarrage
    led_pattern_startup();
    buzzer_pattern_startup();
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 1 : Détection d'appui simple (10 secondes)
    // ========================================================================
    
    led_on(LED_BLUE);
    
    uint32_t counter = 0;
    while (counter < 10000) {
        if (button_is_pressed()) {
            led_on(LED_GREEN);
            buzzer_beep(50);
        } else {
            led_off(LED_GREEN);
        }
        _delay_ms(10);
        counter += 10;
    }
    
    led_off(LED_BLUE);
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 2 : Attendre un appui
    // ========================================================================
    
    // Indique qu'on attend un appui
    led_on(LED_RED);
    led_on(LED_BUILTIN);
    
    button_wait_press();
    
    // Appui détecté
    led_off(LED_RED);
    led_off(LED_BUILTIN);
    led_on(LED_GREEN);
    buzzer_pattern_success();
    _delay_ms(1000);
    led_off(LED_GREEN);
    _delay_ms(500);
    
    // ========================================================================
    // TEST 3 : Attendre relâchement
    // ========================================================================
    
    led_on(LED_BLUE);
    
    button_wait_release();
    
    led_off(LED_BLUE);
    buzzer_beep(100);
    _delay_ms(1000);
    
    // ========================================================================
    // TEST 4 : Compteur d'appuis (10 appuis)
    // ========================================================================
    
    uint8_t press_count = 0;
    
    while (press_count < 10) {
        // Attente d'un appui
        led_on(LED_BUILTIN);
        button_wait_press();
        
        // Appui détecté
        press_count++;
        led_off(LED_BUILTIN);
        
        // Feedback visuel et sonore selon le nombre d'appuis
        if (press_count % 2 == 0) {
            led_blink(LED_GREEN, 100);
            buzzer_beep(100);
        } else {
            led_blink(LED_BLUE, 100);
            buzzer_beep(50);
        }
        
        // Attente relâchement
        button_wait_release();
        _delay_ms(200);
    }
    
    // 10 appuis atteints !
    led_pattern_success();
    buzzer_pattern_success();
    _delay_ms(2000);
    
    // ========================================================================
    // BOUCLE INFINIE : Mode interactif
    // ========================================================================
    
    // LED rouge = en attente d'appui
    // LED verte = bouton appuyé
    
    while (1) {
        if (button_is_pressed()) {
            led_on(LED_GREEN);
            led_off(LED_RED);
            buzzer_beep(30);
            _delay_ms(100);
        } else {
            led_off(LED_GREEN);
            led_on(LED_RED);
        }
        _delay_ms(50);
    }
    
    return 0;
}
