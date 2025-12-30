#include "buzzer.h"

// Initialisation du buzzer
void buzzer_init(void) {
    BUZZER_DDR |= (1 << BUZZER_PIN);    
    buzzer_off();
}

// Allumer le buzzer
void buzzer_on(void) {
    BUZZER_PORT |= (1 << BUZZER_PIN);
}

// Éteindre le buzzer
void buzzer_off(void) {
    BUZZER_PORT &= ~(1 << BUZZER_PIN);
}

// Bip simple (non-bloquant si utilisé avec d'autres fonctions)
void buzzer_beep(uint16_t duration_ms) {
    buzzer_on();
    for (uint16_t i = 0; i < duration_ms; i++) {
        _delay_ms(1);
    }
    buzzer_off();
}

// Bip bloquant (alias pour clarté)
void buzzer_beep_blocking(uint16_t duration_ms) {
    buzzer_beep(duration_ms);
}

// Pattern de démarrage (2 bips courts)
void buzzer_pattern_startup(void) {
    buzzer_beep(100);
    _delay_ms(100);
    buzzer_beep(100);
}

// Pattern d'alerte (3 bips longs)
void buzzer_pattern_alert(void) {
    for (uint8_t i = 0; i < 3; i++) {
        buzzer_beep(200);
        _delay_ms(200);
    }
}

// Pattern de succès (2 bips rapides)
void buzzer_pattern_success(void) {
    buzzer_beep(50);
    _delay_ms(80);
    buzzer_beep(50);
}

// Pattern d'erreur (1 bip long)
void buzzer_pattern_error(void) {
    buzzer_beep(500);
}

// Pattern d'avertissement (bip-bip rapide)
void buzzer_pattern_warning(void) {
    for (uint8_t i = 0; i < 5; i++) {
        buzzer_beep(100);
        _delay_ms(100);
    }
}

// Pattern sirène (alternance rapide)
void buzzer_pattern_siren(uint8_t cycles) {
    for (uint8_t i = 0; i < cycles; i++) {
        // Montée
        for (uint8_t j = 0; j < 10; j++) {
            buzzer_on();
            _delay_ms(5);
            buzzer_off();
            _delay_ms(5);
        }
        
        // Descente
        for (uint8_t j = 0; j < 10; j++) {
            buzzer_on();
            _delay_ms(10);
            buzzer_off();
            _delay_ms(10);
        }
    }
}

// Pattern Morse SOS (... --- ...)
void buzzer_pattern_morse_sos(void) {
    // S (3 courts)
    for (uint8_t i = 0; i < 3; i++) {
        buzzer_beep(100);
        _delay_ms(100);
    }
    
    _delay_ms(200);
    
    // O (3 longs)
    for (uint8_t i = 0; i < 3; i++) {
        buzzer_beep(300);
        _delay_ms(100);
    }
    
    _delay_ms(200);
    
    // S (3 courts)
    for (uint8_t i = 0; i < 3; i++) {
        buzzer_beep(100);
        _delay_ms(100);
    }
}