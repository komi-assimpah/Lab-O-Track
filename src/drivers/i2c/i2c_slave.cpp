#include "i2c_slave.h"
#include <avr/interrupt.h>

static volatile uint8_t g_status = 0;
static volatile uint8_t g_pending_command = CMD_NOP;
static volatile uint8_t g_register_pointer = 0;
static volatile uint8_t g_rx_index = 0;
static volatile uint8_t g_tx_index = 0;
static volatile uint8_t g_rx_buffer[I2C_SLAVE_BUFFER_SIZE];
static volatile char g_tag_id[8] = "OSC-01";
static volatile uint16_t g_timer_left = 0;

void i2c_slave_init(void) {
    TWAR = (I2C_SLAVE_ADDRESS << 1);  // 0x42 → 0x84
    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
}

//TODO: plus commenter cette partie
ISR(TWI_vect) {
    uint8_t status = TWSR & TW_STATUS_MASK;

    switch (status) {
        // ════════════════════════════════════════════════════════════════
        // MODE SLAVE RECEIVER
        // ════════════════════════════════════════════════════════════════
        case TW_SR_SLA_ACK:// Maître veut ÉCRIRE → on se prépare
            g_rx_index = 0;
            break;

        case TW_SR_DATA_ACK:// Maître envoie des données → on stocke
	    if (g_rx_index == 0) {
                g_register_pointer = TWDR;
            } else if (g_register_pointer == REG_COMMAND && g_rx_index == 1) {
                g_pending_command = TWDR;
            }

            if (g_rx_index < I2C_SLAVE_BUFFER_SIZE) {
                g_rx_buffer[g_rx_index++] = TWDR;
            }
            break;

        case TW_SR_STOP:// Maître a fini d'écrire 
            break;

        // ════════════════════════════════════════════════════════════════
        // MODE SLAVE TRANSMITTER
        // ════════════════════════════════════════════════════════════════
        case TW_ST_SLA_ACK: // Maître veut LIRE → on envoie le 1er octet
            
            g_tx_index = 0;

            switch (g_register_pointer) {
                case REG_STATUS:
                    TWDR = g_status;
                    break;

                case REG_TAG_ID:
                    TWDR = g_tag_id[0];
                    g_tx_index = 1;
                    break;

                case REG_TIMER_LEFT:
                    TWDR = (g_timer_left >> 8) & 0xFF;
                    g_tx_index = 1;
                    break;

                default:
                    TWDR = 0xFF;
                    break;
            }
            break;

        case TW_ST_DATA_ACK:  // Maître a reçu le 1er octet → on envoie le suivant
            switch (g_register_pointer) {
                case REG_TAG_ID:
                    if (g_tx_index < 8) {
                        TWDR = g_tag_id[g_tx_index++];
                    } else {
                        TWDR = 0x00;
                    }
                    break;

                case REG_TIMER_LEFT:
                    TWDR = g_timer_left & 0xFF;
                    g_tx_index++;
                    break;

                default:
                    TWDR = 0xFF;
                    break;
            }
            break;

        case TW_ST_DATA_NACK: // Le maitre a fini de lire  
            break;
    }
    TWCR = (1 << TWINT)
         | (1 << TWEA)
         | (1 << TWEN)
         | (1 << TWIE); 
}

void i2c_slave_set_status(uint8_t status) {
    g_status = status;
}

uint8_t i2c_slave_get_pending_command(void) {
    uint8_t cmd = g_pending_command;
    g_pending_command = CMD_NOP;
    return cmd;
}
