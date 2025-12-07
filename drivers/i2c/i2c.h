#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Classe statique pour gérer le module I2C (TWI) sur l'ATmega328P.
 */
class I2CManager {
public:
    // ============================================================================
    // CODES DE STATUT I2C
    // ============================================================================
    // Ces codes sont des constantes C++ désormais, utilisées pour le débogage.

    static constexpr uint8_t STATUS_START = 0x08;      ///< START transmis
    static constexpr uint8_t STATUS_REP_START = 0x10;  ///< Repeated START transmis
    static constexpr uint8_t STATUS_SLAW_ACK = 0x18;   ///< SLA+W transmis, ACK reçu
    static constexpr uint8_t STATUS_SLAW_NACK = 0x20;  ///< SLA+W transmis, NACK reçu
    static constexpr uint8_t STATUS_DATA_ACK = 0x28;   ///< Donnée transmise, ACK reçu
    static constexpr uint8_t STATUS_DATA_NACK = 0x30;  ///< Donnée transmise, NACK reçu

    // ============================================================================
    // FONCTIONS PUBLIQUES (Méthodes Statiques)
    // ============================================================================

    /**
     * @brief Initialise le module I2C (TWI).
     * Configure la fréquence SCL à 100 kHz.
     */
    static void init();

    /**
     * @brief Envoie une condition START sur le bus I2C.
     * @return true si succès, false si erreur ou timeout.
     */
    static bool start();

    /**
     * @brief Envoie une condition STOP sur le bus I2C.
     * Libère le bus.
     */
    static void stop();

    /**
     * @brief Écrit un byte sur le bus I2C.
     * @param data Le byte à envoyer.
     * @return true si ACK reçu, false si NACK ou erreur.
     */
    static bool write(uint8_t data);

    /**
     * @brief Lit un byte depuis le bus I2C avec ACK.
     * @return Le byte lu.
     */
    static uint8_t readAck();

    /**
     * @brief Lit un byte depuis le bus I2C avec NACK.
     * @return Le byte lu.
     */
    static uint8_t readNack();

    /**
     * @brief Obtient le code de statut actuel du bus I2C (pour débogage).
     * @return Le code de statut (registre TWSR masqué).
     */
    static uint8_t getStatus();

private:
    // Constantes internes déplacées dans la classe
    static constexpr uint16_t TIMEOUT = 10000;
    static constexpr uint8_t STATUS_MASK = 0xF8;
};

#endif // I2C_H