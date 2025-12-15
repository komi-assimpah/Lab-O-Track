#include "i2c_slave.h"
#include <avr/interrupt.h>
#include <string.h>


static volatile uint8_t rx_buffer[I2C_SLAVE_RX_BUFFER_SIZE];
static volatile uint8_t tx_buffer[I2C_SLAVE_TX_BUFFER_SIZE];
static volatile uint8_t rx_index = 0;
static volatile uint8_t tx_index = 0;
static volatile uint8_t tx_length = 0;

static volatile uint8_t last_command = 0;
static volatile bool command_pending = false;

static i2c_slave_receive_callback_t receive_callback = NULL;
static i2c_slave_request_callback_t request_callback = NULL;


void i2c_slave_init(uint8_t address) {
    TWAR = (address << 1);
    TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
    
    rx_index = 0;
    tx_index = 0;
    tx_length = 0;
    last_command = 0;
    command_pending = false;
    sei();
}

void i2c_slave_set_receive_callback(i2c_slave_receive_callback_t callback) {
    receive_callback = callback;
}

void i2c_slave_set_request_callback(i2c_slave_request_callback_t callback) {
    request_callback = callback;
}

uint8_t i2c_slave_get_last_command(void) {
    return last_command;
}

bool i2c_slave_command_pending(void) {
    return command_pending;
}

void i2c_slave_command_handled(void) {
    command_pending = false;
}

ISR(TWI_vect) {
    uint8_t status = TWSR & 0xF8;  // Masquer les prescaler bits
    
    switch (status) {
        case TWI_SR_SLA_ACK:
            rx_index = 0;
            TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
            break;
            
        case TWI_SR_DATA_ACK:
            if (rx_index < I2C_SLAVE_RX_BUFFER_SIZE) {
                rx_buffer[rx_index++] = TWDR;
                
                if (rx_index == 1) {
                    last_command = rx_buffer[0];
                    command_pending = true;
                }
            }
            TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
            break;
            
        case TWI_SR_STOP:
            if (receive_callback != NULL && rx_index > 0) {
                receive_callback(rx_buffer[0], 
                                (uint8_t*)&rx_buffer[1], 
                                rx_index - 1);
            }
            rx_index = 0;
            TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
            break;
            
        case TWI_ST_SLA_ACK:
            tx_index = 0;
            tx_length = 0;
            
            if (request_callback != NULL) {
                tx_length = request_callback(last_command, 
                                             (uint8_t*)tx_buffer, 
                                             I2C_SLAVE_TX_BUFFER_SIZE);
            }
            
            if (tx_length > 0) {
                TWDR = tx_buffer[tx_index++];
            } else {
                TWDR = 0xFF;
            }
            
            if (tx_index < tx_length) {
                TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
            } else {
                TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
            }
            break;
            
        case TWI_ST_DATA_ACK:
            if (tx_index < tx_length) {
                TWDR = tx_buffer[tx_index++];
            } else {
                TWDR = 0xFF;
            }
            
            if (tx_index < tx_length) {
                TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
            } else {
                TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
            }
            break;
            
        case TWI_ST_DATA_NACK:
        case TWI_ST_LAST_DATA:
            TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
            break;
            
        default:
            TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
            break;
    }
}
