/*
 * ============================================================================
 * TEST RFID MINIMAL - Uniquement RFID + LCD
 * ============================================================================
 * 
 * Test minimaliste du RFID avec seulement le LCD pour le feedback.
 * 
 * Matériel requis :
 *   - Grove 125KHz RFID Reader sur D0/D1
 *   - LCD Grove 16x2 sur I2C (A4/A5)
 *   - Tags RFID 125KHz
 * 
 * Affichage LCD :
 *   - Ligne 1 : État/Message
 *   - Ligne 2 : ID du tag ou compteur
 * 
 * ⚠️ IMPORTANT : 
 *   - Débranchez le RFID (D0/D1) pendant l'upload !
 *   - Rebranchez-le après l'upload
 */

#include "../drivers/rfid/rfid.h"
#include "../drivers/lcd/lcd.h"
#include "../drivers/i2c/i2c.h"
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // ========================================================================
    // INITIALISATION
    // ========================================================================
    
    // Initialiser I2C et LCD
    i2c_init();
    _delay_ms(100);
    lcd_init();
    
    // Message de démarrage
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Lab-O-Track");
    lcd_set_cursor(1, 0);
    lcd_print("Init RFID...");
    _delay_ms(1500);
    
    // Initialiser le RFID
    rfid_init();
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("RFID Ready!");
    lcd_set_cursor(1, 0);
    lcd_print("Approchez tag");
    _delay_ms(2000);
    
    // ========================================================================
    // VARIABLES
    // ========================================================================
    
    uint8_t tag_id[RFID_DATA_SIZE];
    char tag_string[RFID_DATA_SIZE + 1];
    uint8_t saved_tag[RFID_DATA_SIZE];
    bool first_tag_saved = false;
    uint8_t tag_count = 0;
    
    // ========================================================================
    // BOUCLE PRINCIPALE
    // ========================================================================
    
    while (1) {
        // Afficher état d'attente
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("En attente...");
        lcd_set_cursor(1, 0);
        lcd_print("Tags: ");
        lcd_print_char('0' + (tag_count / 10));
        lcd_print_char('0' + (tag_count % 10));
        
        // Attendre la détection d'un tag
        while (!rfid_available()) {
            _delay_ms(100);
        }
        
        // Tag détecté ! Lire l'ID
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print("Lecture...");
        _delay_ms(300);
        
        if (rfid_read_tag_id(tag_id)) {
            // Lecture réussie !
            rfid_tag_to_string(tag_id, tag_string);
            
            if (!first_tag_saved) {
                // ============================================================
                // PREMIER TAG : Enregistrement
                // ============================================================
                
                for (uint8_t i = 0; i < RFID_DATA_SIZE; i++) {
                    saved_tag[i] = tag_id[i];
                }
                first_tag_saved = true;
                
                lcd_clear();
                lcd_set_cursor(0, 0);
                lcd_print("Tag enregistre");
                lcd_set_cursor(1, 0);
                lcd_print(tag_string);
                _delay_ms(3000);
                
            } else {
                // ============================================================
                // TAGS SUIVANTS : Comparaison
                // ============================================================
                
                lcd_clear();
                lcd_set_cursor(0, 0);
                
                if (rfid_compare_tags(saved_tag, tag_id)) {
                    // Même tag = Autorisé
                    lcd_print("** AUTORISE **");
                    lcd_set_cursor(1, 0);
                    lcd_print(tag_string);
                    _delay_ms(2000);
                } else {
                    // Tag différent = Refusé
                    lcd_print("!! REFUSE !!");
                    lcd_set_cursor(1, 0);
                    lcd_print(tag_string);
                    _delay_ms(2000);
                }
            }
            
            // Incrémenter le compteur
            tag_count++;
            if (tag_count > 99) tag_count = 0;
            
        } else {
            // ============================================================
            // ERREUR DE LECTURE
            // ============================================================
            
            lcd_clear();
            lcd_set_cursor(0, 0);
            lcd_print("Erreur lecture");
            lcd_set_cursor(1, 0);
            lcd_print("Reessayez...");
            _delay_ms(2000);
        }
        
        // Nettoyer le buffer RFID
        rfid_clear();
        _delay_ms(500);
    }
    
    return 0;
}
