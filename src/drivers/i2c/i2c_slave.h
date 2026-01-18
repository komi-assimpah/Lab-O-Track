#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_SLAVE_ADDRESS 0x42
#define I2C_SLAVE_BUFFER_SIZE 24
#define TW_STATUS_MASK 0xF8

// Status codes Slave Receiver
#define TW_SR_SLA_ACK     0x60
#define TW_SR_DATA_ACK    0x80
#define TW_SR_STOP        0xA0

// Status codes Slave Transmitter
#define TW_ST_SLA_ACK     0xA8
#define TW_ST_DATA_ACK    0xB8
#define TW_ST_DATA_NACK   0xC0

// Registres
#define REG_STATUS        0x00
#define REG_TAG_IDS       0x01
#define REG_ADD_TAG        0x03
#define REG_TIMER_LEFT    0x09
#define REG_COMMAND       0x10

// Commandes (written to REG_COMMAND)
#define CMD_NOP           0x00
#define CMD_STOP_ALARM     0x01
#define CMD_CLEAR_TAGS     0x02

// Status flags
#define STATUS_TAG_PRESENT   (1 << 0)
#define STATUS_TIMER_RUNNING (1 << 1)
#define STATUS_ALARM_ACTIVE  (1 << 2)

void i2c_slave_init(void);
void i2c_slave_set_status(uint8_t status);
uint8_t i2c_slave_get_pending_command(void);
bool i2c_slave_check_tag(const char* tag);

#ifdef __cplusplus
}
#endif

#endif
