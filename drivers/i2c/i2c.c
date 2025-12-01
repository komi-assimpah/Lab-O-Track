#include "i2c.h"
#include <util/delay.h>

/*
 * ============================================================================
 * IMPLÉMENTATION DU DRIVER I2C POUR ATMEGA328P
 * ============================================================================
 */

// ============================================================================
// CONSTANTES INTERNES
// ============================================================================

#define I2C_TIMEOUT 10000

// Masque pour extraire le code de statut du registre TWSR
// (on ignore les 3 bits de poids faible qui sont les prescaler bits)
#define TW_STATUS_MASK 0xF8

// ============================================================================
// FONCTIONS PUBLIQUES
// ============================================================================

void i2c_init(void) {
  /*
   * Configuration de la fréquence SCL (horloge I2C)
   *
   * Formule : SCL = F_CPU / (16 + 2 * TWBR * Prescaler)
   *
   * Pour F_CPU = 16 MHz et SCL = 100 kHz :
   * 100000 = 16000000 / (16 + 2 * TWBR * 1)
   * TWBR = 72
   */
  TWBR = 72; // Bit Rate Register
  TWSR = 0;  // Prescaler = 1 (bits TWPS1:0 = 00)

  /*
   * Active le module TWI
   * TWEN = TWI Enable
   */
  TWCR = (1 << TWEN);
}

bool i2c_start(void) {
  /*
   * Envoie une condition START
   *
   * TWINT = TWI Interrupt Flag (doit être mis à 1 pour effacer le flag)
   * TWSTA = TWI START Condition
   * TWEN  = TWI Enable
   */
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

  /*
   * Attend que l'opération soit terminée
   * Le flag TWINT passe à 1 quand c'est fini
   */
  uint16_t timeout = I2C_TIMEOUT;
  while (!(TWCR & (1 << TWINT))) {
    if (--timeout == 0) {
      return false; // Timeout !
    }
  }

  /*
   * Vérifie le code de statut
   * On attend soit START (0x08) soit REPEATED START (0x10)
   */
  uint8_t status = i2c_get_status();
  if (status != I2C_STATUS_START && status != I2C_STATUS_REP_START) {
    return false;
  }

  return true;
}

void i2c_stop(void) {
  /*
   * Envoie une condition STOP
   *
   * TWINT = Efface le flag d'interruption
   * TWSTO = TWI STOP Condition
   * TWEN  = TWI Enable
   */
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

  /*
   * Petite pause pour laisser le STOP se propager
   * (le flag TWSTO est automatiquement effacé par le hardware)
   */
  _delay_us(10);
}

bool i2c_write(uint8_t data) {
  /*
   * Charge le byte à envoyer dans le registre de données
   */
  TWDR = data;

  /*
   * Démarre la transmission
   * TWINT = Efface le flag et démarre l'envoi
   * TWEN  = TWI Enable
   */
  TWCR = (1 << TWINT) | (1 << TWEN);

  /*
   * Attend la fin de la transmission
   */
  uint16_t timeout = I2C_TIMEOUT;
  while (!(TWCR & (1 << TWINT))) {
    if (--timeout == 0) {
      return false; // Timeout !
    }
  }

  /*
   * Vérifie le statut
   * Pour SLA+W : on attend 0x18 (ACK) ou 0x20 (NACK)
   * Pour DATA  : on attend 0x28 (ACK) ou 0x30 (NACK)
   */
  uint8_t status = i2c_get_status();
  if (status != I2C_STATUS_SLAW_ACK && status != I2C_STATUS_DATA_ACK) {
    return false; // NACK ou erreur
  }

  return true;
}

uint8_t i2c_read_ack(void) {
  /*
   * Configure pour recevoir un byte et envoyer ACK
   *
   * TWINT = Efface le flag et démarre la réception
   * TWEA  = TWI Enable Acknowledge (envoie ACK après réception)
   * TWEN  = TWI Enable
   */
  TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);

  /*
   * Attend la réception du byte
   */
  while (!(TWCR & (1 << TWINT)))
    ;

  /*
   * Retourne le byte reçu
   */
  return TWDR;
}

uint8_t i2c_read_nack(void) {
  /*
   * Configure pour recevoir un byte et envoyer NACK
   *
   * TWINT = Efface le flag et démarre la réception
   * TWEN  = TWI Enable
   * (pas de TWEA, donc NACK sera envoyé)
   */
  TWCR = (1 << TWINT) | (1 << TWEN);

  /*
   * Attend la réception du byte
   */
  while (!(TWCR & (1 << TWINT)))
    ;

  /*
   * Retourne le byte reçu
   */
  return TWDR;
}

uint8_t i2c_get_status(void) {
  /*
   * Retourne le code de statut du registre TWSR
   * On masque les 3 bits de poids faible (prescaler)
   */
  return (TWSR & TW_STATUS_MASK);
}
