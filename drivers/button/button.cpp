#include "button.h"

// Le fichier d'en-tête "util/delay.h" définit la fonction _delay_ms()

/**
 * @brief Constructeur - initialise la broche du bouton.
 */
Button::Button() {
    // Configurer en entrée avec pull-up
    // BUTTON_DDR &= ~(1 << BUTTON_PIN);  // Configure la broche en entrée (DDR = 0)
    // BUTTON_PORT |= (1 << BUTTON_PIN); // Active la résistance de Pull-up (PORT = 1)
    
    // Pour éviter le 're-définition' des macros dans le CPP, on les utilise directement.
    // En C++, il est préférable d'utiliser des constantes si possible, mais les registres 
    // AVR nécessitent ces opérations.
    BUTTON_DDR &= ~(1 << BUTTON_PIN);
    BUTTON_PORT |= (1 << BUTTON_PIN);
}

/**
 * @brief Vérifie si le bouton est actuellement appuyé.
 * @return true si le bouton est appuyé (LOW), false sinon.
 */
bool Button::isPressed() const {
    // Le bouton est appuyé si le pin est LOW (pull-up)
    // La lecture se fait sur le registre PIN
    return !(BUTTON_PINREG & (1 << BUTTON_PIN));
}

/**
 * @brief Attend et vérifie l'état 'appuyé' après un délai d'anti-rebond.
 * @return true si le bouton est appuyé après l'anti-rebond.
 */
bool Button::waitPress() {
    // Vérifier l'état initial
    if (isPressed()) {
        // Anti-rebond
        _delay_ms(DEBOUNCE_DELAY_MS);
        
        // Vérifier l'état après l'anti-rebond
        if (isPressed()) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Attend et vérifie l'état 'relâché' après un délai d'anti-rebond.
 * @return true si le bouton est relâché après l'anti-rebond.
 */
bool Button::waitRelease() {
    // Vérifier l'état initial (NON appuyé)
    if (!isPressed()) {
        // Anti-rebond
        _delay_ms(DEBOUNCE_DELAY_MS);
        
        // Vérifier l'état après l'anti-rebond (NON appuyé)
        if (!isPressed()) {
            return true;
        }
    }
    return false;
}