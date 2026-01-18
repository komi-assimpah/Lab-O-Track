#include "i2c_slave.h"
#include <avr/interrupt.h>
#include <string.h>

#define MAX_TAGS 3
#define TAG_LENGTH 16

static volatile uint8_t g_status = 0;
static volatile uint8_t g_pending_command = CMD_NOP;
static volatile uint8_t g_register_pointer = 0;
static volatile uint8_t g_rx_index = 0;
static volatile uint8_t g_tx_index = 0;
static volatile uint8_t g_rx_buffer[I2C_SLAVE_BUFFER_SIZE];
static volatile char g_tag_id[TAG_LENGTH] = "";
static volatile uint16_t g_timer_left = 0;

static char g_allowed_tags[MAX_TAGS][TAG_LENGTH];
static volatile uint8_t g_allowed_tags_count = 0;

void i2c_slave_init(void) {
    TWAR = (I2C_SLAVE_ADDRESS << 1);  // 0x42 → 0x84
    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
    
    // Clear tags initially
    memset((void*)g_allowed_tags, 0, sizeof(g_allowed_tags));
    g_allowed_tags_count = 0;
    /*
    // FOR TESTING ONLY: Add default tag found in logs
    // This allows testing via USB without an active I2C connection from RPi
    // Tag: ␂␂01068DF4B9C7␃␃ (STX + ID + ETX)
    const char* default_tag = "\x02""01068DF4B9C7""\x03";
    strcpy(g_allowed_tags[0], default_tag);
    g_allowed_tags_count = 1;
    */
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
            } else {
                // Config command logic
                if (g_register_pointer == REG_COMMAND && g_rx_index == 1) {
                    uint8_t cmd = TWDR;
                    if (cmd == CMD_CLEAR_TAGS) {
                        g_allowed_tags_count = 0;
                    } else {
                        g_pending_command = cmd;
                    }
                }

                if (g_rx_index < I2C_SLAVE_BUFFER_SIZE) {
                    g_rx_buffer[g_rx_index] = TWDR;
                    g_rx_index++;
                }
            }
            break;

        case TW_SR_STOP:// Maître a fini d'écrire 
            if (g_register_pointer == REG_ADD_TAG && g_rx_index > 1) {
                // We received data for REG_ADD_TAG
                // g_rx_buffer[1] start of data. 
                // Length is g_rx_index - 1
                if (g_allowed_tags_count < MAX_TAGS) {
                    uint8_t len = g_rx_index - 1;
                    if (len >= TAG_LENGTH) len = TAG_LENGTH - 1;
                    
                    uint8_t* src = (uint8_t*)&g_rx_buffer[1];
                    char* dst = g_allowed_tags[g_allowed_tags_count];
                    for(uint8_t i=0; i<len; i++) {
                        dst[i] = (char)src[i];
                    }
                    dst[len] = '\0';
                    g_allowed_tags_count++;
                }
            }
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

                case REG_TAG_IDS:
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
                case REG_TAG_IDS:
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

bool i2c_slave_check_tag(const char* tag) {
    // If no tags configured, maybe deny all? Or allow all? 
    // Let's assume deny all if list is not empty, but if empty maybe it's unconfigured?
    // Safe default: if empty, deny all.

    // Serial.print(F(">>> TAG READ: "));
    // Serial.println(tag);
    
    if (g_allowed_tags_count == 0) return false;
    for (uint8_t i = 0; i < g_allowed_tags_count; i++) {
        if (strcmp(tag, g_allowed_tags[i]) == 0) {
            return true;
        }
    }
    return false;
}
