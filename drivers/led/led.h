#ifndef LED_H
#define LED_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Classe statique pour contrôler les LEDs externes et intégrées.
 *
 * Toutes les méthodes sont statiques car la classe gère un ensemble de ressources matérielles fixes.
 */
class LedManager {
public:
    // Type énuméré pour identifier les LEDs (dans l'espace de noms de la classe)
    enum class LedId : uint8_t {
        RED,
        GREEN,
        BLUE,
        BUILTIN
    };

    // Configuration des pins pour les 3 LEDs externes
    #define LED_RED_PIN     PD2
    #define LED_GREEN_PIN   PD3
    #define LED_BLUE_PIN    PD4
    #define LED_PORT        PORTD
    #define LED_DDR         DDRD

    // LED intégrée (Pin 13) pour debug
    #define LED_BUILTIN_PIN PB5
    #define LED_BUILTIN_DDR DDRB
    #define LED_BUILTIN_PORT PORTB

    // --- Fonctions d'initialisation ---

    /**
     * @brief Configure toutes les broches des LEDs comme sorties et les éteint.
     */
    static void initAll();

    // --- Contrôle individuel des LEDs ---

    /**
     * @brief Allume une LED spécifique.
     */
    static void on(LedId led);

    /**
     * @brief Éteint une LED spécifique.
     */
    static void off(LedId led);

    /**
     * @brief Inverse l'état d'une LED (utilise le registre PIN pour le toggle le cas échéant).
     */
    static void toggle(LedId led);

    /**
     * @brief Fait clignoter une LED pendant une durée spécifiée (bloquant).
     */
    static void blink(LedId led, uint16_t duration_ms);

    // --- Contrôle groupé ---

    /**
     * @brief Allume toutes les LEDs.
     */
    static void allOn();

    /**
     * @brief Éteint toutes les LEDs.
     */
    static void allOff();

    // --- Patterns prédéfinis (Bloquants) ---

    /**
     * @brief Pattern de démarrage (séquence R->V->B puis toutes).
     */
    static void patternStartup();

    /**
     * @brief Pattern d'alerte (Rouge clignotant).
     */
    static void patternAlert();

    /**
     * @brief Pattern de succès (Vert clignotant rapide).
     */
    static void patternSuccess();

    /**
     * @brief Pattern séquence (test de toutes les LEDs).
     */
    static void patternSequence();

private:
    // Méthode utilitaire pour obtenir le registre et le pin (optionnel, facilite l'implémentation)
    // Non implémentée ici pour conserver la compatibilité des macros.
};

#endif // LED_H