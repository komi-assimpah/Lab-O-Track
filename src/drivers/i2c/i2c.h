#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ============================================================================
 * DRIVER I2C POUR ATMEGA328P (Arduino Uno)
 * ============================================================================
 *
 * Ce driver permet de communiquer avec des périphériques I2C (aussi appelé TWI
 * sur les AVR). L'I2C utilise 2 fils : SDA (données) et SCL (horloge).
 *
 * Sur Arduino Uno :
 *   - SDA = A4 (PC4)
 *   - SCL = A5 (PC5)
 *
 * Fréquence I2C : 100 kHz (mode standard)
 */

// ============================================================================
// CODES DE STATUT I2C (pour le débogage)
// ============================================================================

// Ces codes viennent du registre TWSR (TWI Status Register)
#define I2C_STATUS_START 0x08     // START transmis
#define I2C_STATUS_REP_START 0x10 // Repeated START transmis
#define I2C_STATUS_SLAW_ACK 0x18  // SLA+W transmis, ACK reçu
#define I2C_STATUS_SLAW_NACK 0x20 // SLA+W transmis, NACK reçu
#define I2C_STATUS_DATA_ACK 0x28  // Donnée transmise, ACK reçu
#define I2C_STATUS_DATA_NACK 0x30 // Donnée transmise, NACK reçu

// ============================================================================
// FONCTIONS PUBLIQUES
// ============================================================================

/**
 * Initialise le module I2C (TWI)
 *
 * Configure la fréquence SCL à 100 kHz avec F_CPU = 16 MHz
 * Active les pull-ups internes sur SDA et SCL
 */
void i2c_init(void);

/**
 * Envoie une condition START sur le bus I2C
 *
 * Retourne : true si succès, false si erreur
 */
bool i2c_start(void);

/**
 * Envoie une condition STOP sur le bus I2C
 *
 * Libère le bus pour d'autres périphériques
 */
void i2c_stop(void);

/**
 * Écrit un byte sur le bus I2C
 *
 * Paramètres :
 *   - data : le byte à envoyer
 *
 * Retourne : true si ACK reçu, false si NACK ou erreur
 */
bool i2c_write(uint8_t data);

/**
 * Lit un byte depuis le bus I2C avec ACK
 *
 * Envoie un ACK après la lecture pour indiquer qu'on veut
 * continuer à recevoir des données.
 *
 * Retourne : le byte lu
 */
uint8_t i2c_read_ack(void);

/**
 * Lit un byte depuis le bus I2C avec NACK
 *
 * Envoie un NACK après la lecture pour indiquer qu'on ne veut
 * plus recevoir de données (dernier byte).
 *
 * Retourne : le byte lu
 */
uint8_t i2c_read_nack(void);

/**
 * Obtient le code de statut actuel du bus I2C
 *
 * Utile pour le débogage. Compare avec les constantes I2C_STATUS_*
 *
 * Retourne : le code de statut (registre TWSR masqué)
 */
uint8_t i2c_get_status(void);

#ifdef __cplusplus
}
#endif

#endif // I2C_H
