#include <avr/io.h>
#include <util/delay.h>
#include "drivers/led/led.h"
#include "drivers/buzzer/buzzer.h"

// Test individuel du buzzer
void test_buzzer_patterns(void) {
    // 1. Startup
    led_on(LED_GREEN);
    buzzer_pattern_startup();
    led_off(LED_GREEN);
    _delay_ms(500);
    
    // 2. Success
    led_on(LED_GREEN);
    buzzer_pattern_success();
    led_off(LED_GREEN);
    _delay_ms(500);
    
    // 3. Warning
    led_on(LED_BLUE);
    buzzer_pattern_warning();
    led_off(LED_BLUE);
    _delay_ms(500);
    
    // 4. Alert
    led_on(LED_RED);
    buzzer_pattern_alert();
    led_off(LED_RED);
    _delay_ms(500);
    
    // 5. Error
    led_on(LED_RED);
    buzzer_pattern_error();
    led_off(LED_RED);
    _delay_ms(500);
}

// Test coordonné LED + Buzzer
void test_combined_patterns(void) {
    // Alerte équipement manquant
    for (uint8_t i = 0; i < 3; i++) {
        led_on(LED_RED);
        buzzer_on();
        _delay_ms(150);
        
        led_off(LED_RED);
        buzzer_off();
        _delay_ms(150);
    }
    
    _delay_ms(500);
    
    // Équipement retrouvé
    led_pattern_success();
    buzzer_pattern_success();
    
    _delay_ms(1000);
}

// Simulation système de traçabilité
void simulate_tracking_system(void) {
    // Système normal
    led_on(LED_GREEN);
    buzzer_pattern_startup();
    _delay_ms(2000);
    
    // Équipement retiré (warning)
    led_all_off();
    led_on(LED_BLUE);
    buzzer_beep(100);
    _delay_ms(3000);
    
    // Timeout atteint (alerte)
    led_off(LED_BLUE);
    for (uint8_t i = 0; i < 5; i++) {
        led_on(LED_RED);
        buzzer_on();
        _delay_ms(200);
        
        led_off(LED_RED);
        buzzer_off();
        _delay_ms(200);
    }
    
    _delay_ms(1000);
    
    // Équipement retourné
    led_on(LED_GREEN);
    buzzer_pattern_success();
    _delay_ms(2000);
    
    led_all_off();
}

int main(void) {
    // Initialisation des drivers
    led_init_all();
    buzzer_init();
    
    // Séquence de démarrage
    led_pattern_startup();
    buzzer_pattern_startup();
    
    _delay_ms(1000);
    
    while (1) {
        // Test 1 : Patterns du buzzer seul
        test_buzzer_patterns();
        _delay_ms(1000);
        
        // Test 2 : Patterns LED + Buzzer combinés
        test_combined_patterns();
        _delay_ms(1000);
        
        // Test 3 : Simulation système complet
        simulate_tracking_system();
        _delay_ms(2000);
        
        // Test 4 : Sirène (optionnel, peut être bruyant !)
        // Décommente si tu veux tester
        // led_on(LED_RED);
        // buzzer_pattern_siren(2);
        // led_off(LED_RED);
        // _delay_ms(2000);
        
        // Test 5 : SOS Morse
        led_on(LED_RED);
        buzzer_pattern_morse_sos();
        led_off(LED_RED);
        _delay_ms(3000);
    }
    
    return 0;
}