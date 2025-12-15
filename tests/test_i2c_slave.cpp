/*
 * ============================================================================
 * TEST I2C SLAVE - LAB-O-TRACK
 * ============================================================================
 *
 * Ce fichier teste le driver I2C esclave.
 * L'Arduino répond aux commandes du Raspberry Pi.
 *
 * Matériel requis :
 *   - Arduino Uno connecté au Raspberry Pi via I2C
 *   - SDA (A4) -> GPIO 2 du RPi (avec level shifter recommandé)
 *   - SCL (A5) -> GPIO 3 du RPi (avec level shifter recommandé)
 *   - GND commun
 *   - LEDs et buzzer pour feedback
 *
 * Test :
 *   1. Flasher ce programme sur l'Arduino
 *   2. Sur RPi : i2cdetect -y 1 (doit montrer 0x08)
 *   3. Lancer le script Python smbus_master.py
 */

#include "../drivers/smbus/i2c_slave.h"
#include "../drivers/smbus/protocol.h"
#include "../drivers/led/led.h"
#include "../drivers/buzzer/buzzer.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>


static char current_tag[TAG_BUFFER_SIZE] = "0A1B2C3D4E";

static volatile uint8_t current_status = STATUS_OK;

static volatile uint16_t timer_remaining = 0;

// ============================================================================
// CALLBACKS I2C
// ============================================================================

/**
 * Callback appelée quand le maître envoie une commande
 */
void on_receive(uint8_t cmd, uint8_t *data, uint8_t len) {
    // Feedback visuel : commande reçue
    led_blink(LED_BLUE, 50);
    
    // Traitement selon la commande
    switch (cmd) {
        case CMD_GET_STATUS:
            // Rien à faire, la réponse sera envoyée dans on_request
            break;
            
        case CMD_GET_TAG:
            break;
            
        case CMD_GET_TIMER:
            break;
            
        default:
            led_blink(LED_RED, 100);
            break;
    }
}

/**
 * Callback appelée quand le maître demande des données
 */
uint8_t on_request(uint8_t cmd, uint8_t *buffer, uint8_t max_len) {
    // Feedback visuel : données demandées
    led_blink(LED_GREEN, 30);
    
    switch (cmd) {
        case CMD_GET_STATUS:
            // Envoyer le statut (1 byte)
            buffer[0] = current_status;
            return 1;
            
        case CMD_GET_TAG:
            // Envoyer le tag RFID en ASCII (11 bytes avec null)
            if (max_len >= TAG_BUFFER_SIZE) {
                memcpy(buffer, current_tag, TAG_BUFFER_SIZE);
                return TAG_BUFFER_SIZE;
            }
            return 0;
            
        case CMD_GET_TIMER:
            // Envoyer le timer (2 bytes, big-endian)
            buffer[0] = (timer_remaining >> 8) & 0xFF;
            buffer[1] = timer_remaining & 0xFF;
            return 2;
            
        default:
            // Commande inconnue, envoyer 0xFF
            buffer[0] = 0xFF;
            return 1;
    }
}

// ============================================================================
// PROGRAMME PRINCIPAL
// ============================================================================

int main(void) {
    // ------------------------------------------------------------------------
    // INITIALISATION
    // ------------------------------------------------------------------------
    
    led_init_all();
    buzzer_init();
    
    led_pattern_startup();
    buzzer_pattern_startup();
    _delay_ms(500);
    
    i2c_slave_init(I2C_SLAVE_ADDRESS);
    
    // Définir les callbacks
    i2c_slave_set_receive_callback(on_receive);
    i2c_slave_set_request_callback(on_request);
    
    // Signal : prêt à recevoir
    led_on(LED_GREEN);
    buzzer_beep(200);
    _delay_ms(500);
    led_off(LED_GREEN);
    
    // ------------------------------------------------------------------------
    // BOUCLE PRINCIPALE
    // ------------------------------------------------------------------------
    
    uint8_t demo_counter = 0;
    
    while (1) {
        // Simulation : changer le statut toutes les 10 secondes
        demo_counter++;
        
        if (demo_counter >= 100) {  // 100 x 100ms = 10 secondes
            demo_counter = 0;
            
            // Cycle à travers les statuts pour la démo
            switch (current_status) {
                case STATUS_OK:
                    current_status = STATUS_TAG_PRESENT;
                    timer_remaining = 900;  // 15 minutes
                    led_blink(LED_GREEN, 200);
                    break;
                    
                case STATUS_TAG_PRESENT:
                    current_status = STATUS_TAG_MISSING;
                    timer_remaining = 300;  // 5 minutes restantes
                    led_blink(LED_BLUE, 200);
                    break;
                    
                case STATUS_TAG_MISSING:
                    current_status = STATUS_ALERT;
                    timer_remaining = 0;
                    led_blink(LED_RED, 200);
                    buzzer_beep(500);
                    break;
                    
                case STATUS_ALERT:
                    current_status = STATUS_OK;
                    timer_remaining = 0;
                    led_blink(LED_GREEN, 200);
                    // Changer le tag pour la prochaine démo
                    strcpy(current_tag, "1122334455");
                    break;
            }
        }
        
        // Décrémenter le timer si actif
        if (timer_remaining > 0 && demo_counter % 10 == 0) {
            timer_remaining--;
        }
        
        // Indicateur de vie : LED builtin clignote
        if (demo_counter % 50 == 0) {
            led_toggle(LED_BUILTIN);
        }
        
        _delay_ms(100);
    }
    
    return 0;
}
