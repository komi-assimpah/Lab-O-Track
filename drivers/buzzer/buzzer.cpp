#include "buzzer.h"

// Le fichier d'en-tête "util/delay.h" définit la fonction _delay_ms()

/**
 * @brief Constructeur - initialise la broche du buzzer comme sortie.
 */
Buzzer::Buzzer() {
    // Configurer le pin en sortie (DDR = 1)
    BUZZER_DDR |= (1 << BUZZER_PIN);    
    // Éteindre le buzzer au démarrage
    off(); 
}

// --- Fonctions de base ---

/**
 * @brief Active le buzzer (met la broche à HIGH).
 */
void Buzzer::on() const {
    BUZZER_PORT |= (1 << BUZZER_PIN);
}

/**
 * @brief Désactive le buzzer (met la broche à LOW).
 */
void Buzzer::off() const {
    BUZZER_PORT &= ~(1 << BUZZER_PIN);
}

/**
 * @brief Génère un bip de la durée spécifiée. Bloquant.
 */
void Buzzer::beep(uint16_t duration_ms) const {
    on();
    // La boucle for est remplacée par un seul appel à _delay_ms()
    // pour être plus direct (la version C était équivalente mais plus verbeuse).
    _delay_ms(duration_ms); 
    off();
}

/**
 * @brief Alias pour beep(duration_ms) pour plus de clarté.
 */
void Buzzer::beepBlocking(uint16_t duration_ms) const {
    beep(duration_ms);
}

// --- Patterns Sonores Prédéfinis ---

/**
 * @brief Pattern de démarrage (2 bips courts).
 */
void Buzzer::patternStartup() const {
    beep(100);
    _delay_ms(100);
    beep(100);
}

/**
 * @brief Pattern d'alerte (3 bips longs).
 */
void Buzzer::patternAlert() const {
    for (uint8_t i = 0; i < 3; i++) {
        beep(200);
        _delay_ms(200);
    }
}

/**
 * @brief Pattern de succès (2 bips rapides).
 */
void Buzzer::patternSuccess() const {
    beep(50);
    _delay_ms(80);
    beep(50);
}

/**
 * @brief Pattern d'erreur (1 bip long).
 */
void Buzzer::patternError() const {
    beep(500);
}

/**
 * @brief Pattern d'avertissement (bip-bip rapide).
 */
void Buzzer::patternWarning() const {
    for (uint8_t i = 0; i < 5; i++) {
        beep(100);
        _delay_ms(100);
    }
}

// --- Patterns Complexes ---

/**
 * @brief Pattern de sirène (alternance rapide ON/OFF).
 */
void Buzzer::patternSiren(uint8_t cycles) const {
    for (uint8_t i = 0; i < cycles; i++) {
        // Montée
        // Le code C utilisait une boucle pour alterner ON/OFF,
        // ici on simule la variation en jouant sur les délais.
        // NOTE: Ceci ne génère PAS de fréquence (PWM) mais un simple signal ON/OFF.
        for (uint8_t j = 0; j < 10; j++) {
            on();
            _delay_ms(5);
            off();
            _delay_ms(5);
        }
        
        // Descente
        for (uint8_t j = 0; j < 10; j++) {
            on();
            _delay_ms(10);
            off();
            _delay_ms(10);
        }
    }
}

/**
 * @brief Pattern Morse SOS (... --- ...).
 */
void Buzzer::patternMorseSos() const {
    const uint16_t SHORT_DUR = 100; // Durée d'un 'point'
    const uint16_t LONG_DUR = 300;  // Durée d'un 'trait' (3x point)
    const uint16_t CHAR_SEP = 200;  // Séparation entre les lettres
    const uint16_t DOT_SEP = 100;   // Séparation entre les points/traits (1x point)

    // S (3 courts)
    for (uint8_t i = 0; i < 3; i++) {
        beep(SHORT_DUR);
        _delay_ms(DOT_SEP); // Séparation entre les points
    }
    
    _delay_ms(CHAR_SEP); // Séparation entre S et O
    
    // O (3 longs)
    for (uint8_t i = 0; i < 3; i++) {
        beep(LONG_DUR);
        _delay_ms(DOT_SEP); // Séparation entre les traits
    }
    
    _delay_ms(CHAR_SEP); // Séparation entre O et S
    
    // S (3 courts)
    for (uint8_t i = 0; i < 3; i++) {
        beep(SHORT_DUR);
        _delay_ms(DOT_SEP); // Séparation entre les points
    }
}