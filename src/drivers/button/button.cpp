#include "button.h"

void button_init(void) {
    // Configurer en entrée avec pull-up
    BUTTON_DDR &= ~(1 << BUTTON_PIN);
    BUTTON_PORT |= (1 << BUTTON_PIN);
}

bool button_is_pressed(void) {
    // Le bouton est appuyé si le pin est LOW (pull-up)
    return !(BUTTON_PINREG & (1 << BUTTON_PIN));
}

bool button_wait_press(void) {
    // Attendre que le bouton soit appuyé
    if (button_is_pressed()) {
        _delay_ms(DEBOUNCE_DELAY_MS);
        if (button_is_pressed()) {
            return true;
        }
    }
    return false;
}

bool button_wait_release(void) {
    // Attendre que le bouton soit relâché
    if (!button_is_pressed()) {
        _delay_ms(DEBOUNCE_DELAY_MS);
        if (!button_is_pressed()) {
            return true;
        }
    }
    return false;
}