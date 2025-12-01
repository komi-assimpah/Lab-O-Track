#include "buzzer.h"

void buzzer_init(void) {
    BUZZER_DDR |= (1 << BUZZER_PIN);
    buzzer_off();
}

void buzzer_on(void) {
    BUZZER_PORT |= (1 << BUZZER_PIN);
}

void buzzer_off(void) {
    BUZZER_PORT &= ~(1 << BUZZER_PIN);
}

void buzzer_beep(uint16_t duration_ms) {
    buzzer_on();
    for (uint16_t i = 0; i < duration_ms; i++) {
        _delay_ms(1);
    }
    buzzer_off();
}

void buzzer_alert_pattern(void) {
    // Pattern d'alerte : 3 bips courts
    for (uint8_t i = 0; i < 3; i++) {
        buzzer_beep(150);
        _delay_ms(150);
    }
}

void buzzer_success_pattern(void) {
    // Pattern de succès : 2 bips rapides
    buzzer_beep(50);
    _delay_ms(50);
    buzzer_beep(50);
}