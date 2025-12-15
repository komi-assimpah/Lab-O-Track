/*
 * ============================================================================
 * DRIVER I2C ESCLAVE POUR ATMEGA328P (Arduino Uno)
 * ============================================================================
 *
 * Ce driver configure l'Arduino en mode ESCLAVE I2C pour recevoir des
 * commandes du Raspberry Pi (maître).
 *
 * Contrairement au driver i2c.h qui est en mode MAÎTRE, ce driver :
 *   - Écoute passivement sur le bus I2C
 *   - Répond aux requêtes du maître via interruptions
 *   - Utilise des callbacks pour traiter les commandes
 *
 * Sur Arduino Uno :
 *   - SDA = A4 (PC4)
 *   - SCL = A5 (PC5)
 */

#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include "protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_SLAVE_RX_BUFFER_SIZE 16
#define I2C_SLAVE_TX_BUFFER_SIZE 16


// ============================================================================
// CODES DE STATUT TWI ESCLAVE
// ============================================================================

#define TWI_SR_SLA_ACK      0x60
#define TWI_SR_DATA_ACK     0x80
#define TWI_SR_STOP         0xA0

#define TWI_ST_SLA_ACK      0xA8
#define TWI_ST_DATA_ACK     0xB8
#define TWI_ST_DATA_NACK    0xC0
#define TWI_ST_LAST_DATA    0xC8



typedef void (*i2c_slave_receive_callback_t)(uint8_t cmd, uint8_t *data, uint8_t len);

typedef uint8_t (*i2c_slave_request_callback_t)(uint8_t cmd, uint8_t *buffer, uint8_t max_len);

void i2c_slave_init(uint8_t address);

void i2c_slave_set_receive_callback(i2c_slave_receive_callback_t callback);

void i2c_slave_set_request_callback(i2c_slave_request_callback_t callback);





uint8_t i2c_slave_get_last_command(void);

bool i2c_slave_command_pending(void);

void i2c_slave_command_handled(void);

#ifdef __cplusplus
}
#endif

#endif // I2C_SLAVE_H
