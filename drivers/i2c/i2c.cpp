#include "i2c.h"
#include <util/delay.h>

// ============================================================================
// IMPLÉMENTATION DES MÉTHODES STATIQUES DU DRIVER I2C
// ============================================================================

/**
 * @brief Initialise le module I2C (TWI).
 */
void I2CManager::init() {
    /*
     * Configuration de la fréquence SCL (100 kHz pour F_CPU = 16 MHz)
     * TWBR = 72, Prescaler = 1 (TWSR = 0)
     */
    TWBR = 72;
    TWSR = 0;

    /*
     * Active le module TWI (TWEN)
     */
    TWCR = (1 << TWEN);
}

/**
 * @brief Envoie une condition START sur le bus I2C.
 */
bool I2CManager::start() {
    /*
     * Envoie une condition START
     * TWINT = Efface le flag
     * TWSTA = TWI START Condition
     * TWEN  = TWI Enable
     */
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    /*
     * Attend que l'opération soit terminée (TWINT passe à 1)
     */
    uint16_t timeout = TIMEOUT;
    while (!(TWCR & (1 << TWINT))) {
        if (--timeout == 0) {
            return false; // Timeout !
        }
    }

    /*
     * Vérifie le code de statut
     */
    uint8_t status = getStatus();
    if (status != STATUS_START && status != STATUS_REP_START) {
        return false;
    }

    return true;
}

/**
 * @brief Envoie une condition STOP sur le bus I2C.
 */
void I2CManager::stop() {
    /*
     * Envoie une condition STOP
     * TWINT = Efface le flag
     * TWSTO = TWI STOP Condition
     * TWEN  = TWI Enable
     */
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

    /*
     * Petite pause pour laisser le STOP se propager
     */
    _delay_us(10);
}

/**
 * @brief Écrit un byte sur le bus I2C.
 */
bool I2CManager::write(uint8_t data) {
    /*
     * Charge le byte à envoyer et démarre la transmission
     */
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);

    /*
     * Attend la fin de la transmission
     */
    uint16_t timeout = TIMEOUT;
    while (!(TWCR & (1 << TWINT))) {
        if (--timeout == 0) {
            return false; // Timeout !
        }
    }

    /*
     * Vérifie le statut (ACK attendu)
     */
    uint8_t status = getStatus();
    if (status != STATUS_SLAW_ACK && status != STATUS_DATA_ACK) {
        return false; // NACK ou erreur
    }

    return true;
}

/**
 * @brief Lit un byte depuis le bus I2C avec ACK.
 */
uint8_t I2CManager::readAck() {
    /*
     * Configure pour recevoir un byte et envoyer ACK (TWEA)
     */
    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);

    /*
     * Attend la réception
     */
    while (!(TWCR & (1 << TWINT)))
        ;

    /*
     * Retourne le byte reçu
     */
    return TWDR;
}

/**
 * @brief Lit un byte depuis le bus I2C avec NACK.
 */
uint8_t I2CManager::readNack() {
    /*
     * Configure pour recevoir un byte et envoyer NACK (sans TWEA)
     */
    TWCR = (1 << TWINT) | (1 << TWEN);

    /*
     * Attend la réception
     */
    while (!(TWCR & (1 << TWINT)))
        ;

    /*
     * Retourne le byte reçu
     */
    return TWDR;
}

/**
 * @brief Obtient le code de statut actuel du bus I2C.
 */
uint8_t I2CManager::getStatus() {
    /*
     * Retourne le code de statut masqué
     */
    return (TWSR & STATUS_MASK);
}