#include <avr/io.h>
#include <util/delay.h>
#include "drivers/led/led.h"

int main(void) {
    // Initialisation
    led_init_all();
    
    // Pattern de démarrage
    led_pattern_startup();
    
    _delay_ms(1000);
    
    while (1) {
        // Test complet
        led_pattern_sequence();
        
        _delay_ms(2000);
        
        // Test patterns
        led_pattern_alert();
        _delay_ms(500);
        
        led_pattern_success();
        _delay_ms(500);
        
        // Test individuel
        led_on(LED_RED);
        _delay_ms(300);
        led_off(LED_RED);
        
        led_on(LED_GREEN);
        _delay_ms(300);
        led_off(LED_GREEN);
        
        led_on(LED_BLUE);
        _delay_ms(300);
        led_off(LED_BLUE);
        
        // Test toggle
        for (uint8_t i = 0; i < 10; i++) {
            led_toggle(LED_BUILTIN);
            _delay_ms(100);
        }
        
        _delay_ms(1000);
    }
    
    return 0;
}