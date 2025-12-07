#include "led.h"

// --- Fonctions d'initialisation ---

void LedManager::initAll() {
    // Configurer les pins en sortie
    // Externes (PORTD)
    LED_DDR |= (1 << LED_RED_PIN) | (1 << LED_GREEN_PIN) | (1 << LED_BLUE_PIN);
    // Intégrée (PORTB)
    LED_BUILTIN_DDR |= (1 << LED_BUILTIN_PIN);
    
    // Éteindre toutes les LEDs
    allOff();
}

// --- Contrôle individuel des LEDs ---

void LedManager::on(LedId led) {
    switch (led) {
        case LedId::RED:
            LED_PORT |= (1 << LED_RED_PIN);
            break;
        case LedId::GREEN:
            LED_PORT |= (1 << LED_GREEN_PIN);
            break;
        case LedId::BLUE:
            LED_PORT |= (1 << LED_BLUE_PIN);
            break;
        case LedId::BUILTIN:
            LED_BUILTIN_PORT |= (1 << LED_BUILTIN_PIN);
            break;
    }
}

void LedManager::off(LedId led) {
    switch (led) {
        case LedId::RED:
            LED_PORT &= ~(1 << LED_RED_PIN);
            break;
        case LedId::GREEN:
            LED_PORT &= ~(1 << LED_GREEN_PIN);
            break;
        case LedId::BLUE:
            LED_PORT &= ~(1 << LED_BLUE_PIN);
            break;
        case LedId::BUILTIN:
            LED_BUILTIN_PORT &= ~(1 << LED_BUILTIN_PIN);
            break;
    }
}

void LedManager::toggle(LedId led) {
    // En C++, l'utilisation du registre PIN pour toggler une broche est une méthode
    // plus propre et atomique que l'opération XOR sur PORTx, bien que XOR soit aussi commun.
    // L'implémentation originale utilisait XOR, que nous conservons pour la portabilité du code.
    switch (led) {
        case LedId::RED:
            LED_PORT ^= (1 << LED_RED_PIN);
            break;
        case LedId::GREEN:
            LED_PORT ^= (1 << LED_GREEN_PIN);
            break;
        case LedId::BLUE:
            LED_PORT ^= (1 << LED_BLUE_PIN);
            break;
        case LedId::BUILTIN:
            LED_BUILTIN_PORT ^= (1 << LED_BUILTIN_PIN);
            break;
    }
}

void LedManager::blink(LedId led, uint16_t duration_ms) {
    on(led);
    _delay_ms(duration_ms); // Remplacer la boucle for par un appel direct pour la clarté
    off(led);
}

// --- Contrôle groupé ---

void LedManager::allOn() {
    on(LedId::RED);
    on(LedId::GREEN);
    on(LedId::BLUE);
    on(LedId::BUILTIN);
}

void LedManager::allOff() {
    off(LedId::RED);
    off(LedId::GREEN);
    off(LedId::BLUE);
    off(LedId::BUILTIN);
}

// --- Patterns prédéfinis ---

void LedManager::patternStartup() {
    allOff();
    
    // Séquence rouge -> vert -> bleu
    blink(LedId::RED, 200);
    _delay_ms(50);
    blink(LedId::GREEN, 200);
    _delay_ms(50);
    blink(LedId::BLUE, 200);
    _delay_ms(50);
    
    // Toutes ensemble
    allOn();
    _delay_ms(300);
    allOff();
}

void LedManager::patternAlert() {
    allOff();
    
    // Rouge et LED intégrée clignotent
    for (uint8_t i = 0; i < 3; i++) {
        on(LedId::RED);
        on(LedId::BUILTIN);
        _delay_ms(150);
        off(LedId::RED);
        off(LedId::BUILTIN);
        _delay_ms(150);
    }
}

void LedManager::patternSuccess() {
    allOff();
    
    // Vert clignotant rapide
    for (uint8_t i = 0; i < 2; i++) {
        on(LedId::GREEN);
        _delay_ms(80);
        off(LedId::GREEN);
        _delay_ms(80);
    }
}

void LedManager::patternSequence() {
    allOff();
    
    // Rouge
    on(LedId::RED);
    on(LedId::BUILTIN);
    _delay_ms(500);
    
    // Vert
    allOff();
    on(LedId::GREEN);
    on(LedId::BUILTIN);
    _delay_ms(500);
    
    // Bleu
    allOff();
    on(LedId::BLUE);
    on(LedId::BUILTIN);
    _delay_ms(500);
    
    // Toutes
    allOn();
    _delay_ms(500);
    
    // Éteintes
    allOff();
    _delay_ms(500);
    
    // Clignotement rapide en séquence
    for (uint8_t i = 0; i < 5; i++) {
        blink(LedId::RED, 100);
        blink(LedId::GREEN, 100);
        blink(LedId::BLUE, 100);
    }
}