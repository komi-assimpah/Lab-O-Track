/*
 * ============================================================================
 * TEST RFID DRIVER - Grove 125KHz RFID Reader
 * ============================================================================
 * 
 * Ce fichier teste le driver RFID Grove 125KHz.
 * 
 * Matériel requis :
 *   - Grove 125KHz RFID Reader connecté sur UART (RX=D0, TX=D1)
 *   - Tags RFID 125KHz
 *   - LEDs pour indication visuelle
 *   - Buzzer pour feedback sonore
 *   - LCD pour affichage des IDs
 * 
 * Tests effectués :
 *   1. Initialisation RFID + UART
 *   2. Détection de présence de tag
 *   3. Lecture complète du tag (14 bytes)
 *   4. Extraction de l'ID (10 bytes)
 *   5. Affichage sur LCD
 *   6. Comparaison de tags
 *   7. Mode surveillance continue
 * 
 * ⚠️ IMPORTANT : 
 *   - Le port série (D0/D1) est utilisé pour le RFID
 *   - Vous ne pouvez PAS utiliser Serial Monitor simultanément
 *   - Débranchez le câble USB après upload pour tester
 */

#include "../drivers/rfid/rfid.h"
#include "../drivers/lcd/lcd.h"
#include "../drivers/i2c/i2c.h"
#include "../drivers/led/led.h"
#include "../drivers/buzzer/buzzer.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

int main(void) {
    // ========================================================================
    // INITIALISATION
    // ========================================================================
    
    led_init_all();
    buzzer_init();
    i2c_init();
    lcd_init();
    
    led_pattern_startup();
    buzzer_pattern_startup();
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Init RFID...");
    _delay_ms(1000);
    
    // Initialiser le RFID
    rfid_init();
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("RFID Ready!");
    led_on(LED_GREEN);
    buzzer_pattern_success();
    _delay_ms(1000);
    led_off(LED_GREEN);
    
    // ========================================================================
    // TEST 1 : Attente et détection de tag
    // ========================================================================
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Approchez tag");
    lcd_set_cursor(1, 0);
    lcd_print("RFID...");
    
    uint8_t tag_buffer[RFID_BUFFER_SIZE];
    uint8_t tag_id[RFID_DATA_SIZE];
    char tag_string[RFID_DATA_SIZE + 1];
    
    // Attendre la détection d'un premier tag
    led_on(LED_BLUE);
    while (!rfid_available()) {
        led_toggle(LED_BLUE);
        _delay_ms(200);
    }
    led_off(LED_BLUE);
    
    // ========================================================================
    // TEST 2 : Lecture complète du tag
    // ========================================================================
    
    uint8_t count = rfid_read(tag_buffer);
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Bytes lus:");
    lcd_set_cursor(1, 0);
    
    // Afficher le nombre de bytes
    if (count >= 10) {
        lcd_print_char('0' + (count / 10));
    }
    lcd_print_char('0' + (count % 10));
    
    led_on(LED_GREEN);
    buzzer_beep(200);
    _delay_ms(2000);
    led_off(LED_GREEN);
    
    // ========================================================================
    // TEST 3 : Vérification du format (STX/ETX)
    // ========================================================================
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    
    if (count >= RFID_TAG_SIZE && tag_buffer[0] == RFID_STX && tag_buffer[13] == RFID_ETX) {
        lcd_print("Format: OK");
        led_on(LED_GREEN);
        buzzer_beep(100);
        _delay_ms(500);
        buzzer_beep(100);
    } else {
        lcd_print("Format: ERREUR");
        led_on(LED_RED);
        buzzer_pattern_error();
        _delay_ms(2000);
        led_off(LED_RED);
    }
    
    _delay_ms(1500);
    led_off(LED_GREEN);
    
    // ========================================================================
    // TEST 4 : Extraction et affichage de l'ID
    // ========================================================================
    
    rfid_clear();
    _delay_ms(500);
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Approchez tag");
    
    // Attendre un nouveau tag
    led_on(LED_BLUE);
    while (!rfid_available()) {
        _delay_ms(50);
    }
    led_off(LED_BLUE);
    
    // Lire l'ID du tag
    if (rfid_read_tag_id(tag_id)) {
        // Convertir en chaîne
        rfid_tag_to_string(tag_id, tag_string);
        
        // Afficher sur LCD
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("ID Tag:");
        lcd_set_cursor(1, 0);
        lcd_print(tag_string);
        
        led_on(LED_GREEN);
        buzzer_pattern_success();
        _delay_ms(3000);
        led_off(LED_GREEN);
    } else {
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("Erreur lecture");
        led_on(LED_RED);
        buzzer_pattern_error();
        _delay_ms(2000);
        led_off(LED_RED);
    }
    
    // ========================================================================
    // TEST 5 : Sauvegarde du premier tag (pour comparaison)
    // ========================================================================
    
    uint8_t saved_tag[RFID_DATA_SIZE];
    memcpy(saved_tag, tag_id, RFID_DATA_SIZE);
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Tag enregistre");
    lcd_set_cursor(1, 0);
    lcd_print(tag_string);
    
    buzzer_beep(100);
    _delay_ms(2000);
    
    // ========================================================================
    // TEST 6 : Comparaison de tags
    // ========================================================================
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Testez un autre");
    lcd_set_cursor(1, 0);
    lcd_print("tag...");
    
    rfid_clear();
    
    // Attendre un nouveau tag
    led_on(LED_BLUE);
    while (!rfid_available()) {
        _delay_ms(50);
    }
    led_off(LED_BLUE);
    
    uint8_t new_tag[RFID_DATA_SIZE];
    if (rfid_read_tag_id(new_tag)) {
        rfid_tag_to_string(new_tag, tag_string);
        
        lcd_clear();
        lcd_set_cursor(0, 0);
        
        if (rfid_compare_tags(saved_tag, new_tag)) {
            lcd_print("MEME TAG!");
            led_on(LED_GREEN);
            buzzer_pattern_success();
            _delay_ms(2000);
            led_off(LED_GREEN);
        } else {
            lcd_print("TAG DIFFERENT");
            led_on(LED_BLUE);
            buzzer_beep(300);
            _delay_ms(1000);
            led_off(LED_BLUE);
        }
        
        lcd_set_cursor(1, 0);
        lcd_print(tag_string);
        _delay_ms(3000);
    }
    
    // ========================================================================
    // BOUCLE INFINIE : Mode surveillance continue
    // ========================================================================
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Mode continu");
    _delay_ms(1500);
    
    uint8_t tag_count = 0;
    
    while (1) {
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("En attente...");
        lcd_set_cursor(1, 0);
        lcd_print("Tags: ");
        lcd_print_char('0' + tag_count);
        
        // LED bleue indique attente
        led_on(LED_BLUE);
        led_off(LED_GREEN);
        led_off(LED_RED);
        
        // Attendre un tag
        while (!rfid_available()) {
            _delay_ms(100);
        }
        
        // Tag détecté !
        led_off(LED_BLUE);
        
        if (rfid_read_tag_id(tag_id)) {
            rfid_tag_to_string(tag_id, tag_string);
            
            // Vérifier si c'est le tag enregistré
            if (rfid_compare_tags(saved_tag, tag_id)) {
                // Tag autorisé
                lcd_clear();
                lcd_set_cursor(0, 0);
                lcd_print("ACCES OK");
                lcd_set_cursor(1, 0);
                lcd_print(tag_string);
                
                led_on(LED_GREEN);
                buzzer_pattern_success();
                _delay_ms(2000);
                led_off(LED_GREEN);
            } else {
                // Tag non autorisé
                lcd_clear();
                lcd_set_cursor(0, 0);
                lcd_print("ACCES REFUSE");
                lcd_set_cursor(1, 0);
                lcd_print(tag_string);
                
                led_on(LED_RED);
                buzzer_pattern_error();
                _delay_ms(2000);
                led_off(LED_RED);
            }
            
            tag_count++;
            if (tag_count > 9) tag_count = 0;
        } else {
            // Erreur de lecture
            lcd_clear();
            lcd_set_cursor(0, 0);
            lcd_print("Erreur lecture");
            
            led_blink(LED_RED, 100);
            buzzer_beep(100);
            _delay_ms(1000);
        }
        
        rfid_clear();
        _delay_ms(500);
    }
    
    return 0;
}
