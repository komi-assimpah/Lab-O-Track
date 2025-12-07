#ifndef BUZZER_H
#define BUZZER_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

/**
 * @brief Classe pour contrôler un Buzzer Piézo (actif ou passif en mode simple ON/OFF).
 *
 * Toutes les fonctions sont bloquantes car elles utilisent _delay_ms().
 */
class Buzzer {
public:
    // Configuration : Buzzer sur D6
    // Les registres AVR sont conservés comme des macros
    #define BUZZER_PIN  PD6
    #define BUZZER_PORT PORTD
    #define BUZZER_DDR  DDRD

    /**
     * @brief Constructeur - initialise la broche du buzzer comme sortie.
     */
    Buzzer();

    // --- Fonctions de base ---

    /**
     * @brief Active le buzzer (met la broche à HIGH).
     */
    void on() const;

    /**
     * @brief Désactive le buzzer (met la broche à LOW).
     */
    void off() const;

    /**
     * @brief Génère un bip de la durée spécifiée. Bloquant.
     * @param duration_ms Durée du bip en millisecondes.
     */
    void beep(uint16_t duration_ms) const;

    /**
     * @brief Alias pour beep(duration_ms) pour plus de clarté.
     * @param duration_ms Durée du bip en millisecondes.
     */
    void beepBlocking(uint16_t duration_ms) const;

    // --- Patterns Sonores Prédéfinis ---

    /**
     * @brief Pattern de démarrage (2 bips courts).
     */
    void patternStartup() const;

    /**
     * @brief Pattern d'alerte (3 bips longs).
     */
    void patternAlert() const;

    /**
     * @brief Pattern de succès (2 bips rapides).
     */
    void patternSuccess() const;

    /**
     * @brief Pattern d'erreur (1 bip long).
     */
    void patternError() const;

    /**
     * @brief Pattern d'avertissement (bip-bip rapide répété 5 fois).
     */
    void patternWarning() const;

    // --- Patterns Complexes ---

    /**
     * @brief Pattern de sirène (alternance rapide ON/OFF).
     * @param cycles Nombre de cycles de montée/descente à effectuer.
     */
    void patternSiren(uint8_t cycles) const;

    /**
     * @brief Pattern Morse SOS (... --- ...).
     */
    void patternMorseSos() const;
};

#endif // BUZZER_H