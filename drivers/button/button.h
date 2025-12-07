#ifndef BUTTON_H
#define BUTTON_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h> // Peut être omis en C++, mais conservé pour l'environnement AVR si nécessaire

/**
 * @brief Classe pour gérer un bouton avec résistance de PULL-UP intégrée
 *
 * Le bouton est connecté entre la broche (pin) de l'AVR et la masse (GND).
 * L'état 'appuyé' est lu comme LOW.
 */
class Button {
public:
    // Configuration : Bouton sur D7
    // Les registres sont conservés comme des macros pour la clarté et la compatibilité AVR
    #define BUTTON_PIN PD7
    #define BUTTON_PORT PORTD
    #define BUTTON_DDR DDRD
    #define BUTTON_PINREG PIND

    // Délai de Debounce
    static constexpr int DEBOUNCE_DELAY_MS = 50;

    /**
     * @brief Constructeur - initialise la broche du bouton.
     */
    Button();

    /**
     * @brief Vérifie si le bouton est actuellement appuyé.
     * @return true si le bouton est appuyé (LOW), false sinon.
     */
    bool isPressed() const;

    /**
     * @brief Attend et vérifie l'état 'appuyé' après un délai d'anti-rebond.
     * @return true si le bouton est appuyé après l'anti-rebond.
     */
    bool waitPress();

    /**
     * @brief Attend et vérifie l'état 'relâché' après un délai d'anti-rebond.
     * @return true si le bouton est relâché après l'anti-rebond.
     */
    bool waitRelease();
};

#endif // BUTTON_H