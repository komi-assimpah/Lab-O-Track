#include "rfid.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

// Buffer interne pour stocker les données RFID
static uint8_t rfid_buffer[RFID_BUFFER_SIZE];

/*
 * Initialise l'UART pour le module RFID
 * Baud rate : 9600
 * Format : 8 bits de données, pas de parité, 1 bit de stop
 */
void rfid_init(void) {
    // Calcul du baud rate : UBRR = (F_CPU / (16 * BAUD)) - 1
    // Pour 16MHz et 9600 bauds : UBRR = 103
    uint16_t ubrr = (F_CPU / 16 / 9600) - 1;
    
    // Configurer le baud rate
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    
    // Activer le récepteur (RX)
    UCSR0B = (1 << RXEN0);
    
    // Format : 8 bits de données, 1 bit de stop, pas de parité
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    
    // Vider le buffer au démarrage
    rfid_clear();
}

/*
 * Vérifie si des données sont disponibles sur l'UART
 */
bool rfid_available(void) {
    return (UCSR0A & (1 << RXC0));
}

/*
 * Lit un byte depuis l'UART
 */
uint8_t rfid_read_byte(void) {
    // Attendre qu'un byte soit disponible
    while (!rfid_available());
    
    // Lire le byte depuis le registre UDR0
    return UDR0;
}

/*
 * Lit un tag RFID complet (14 bytes)
 */
uint8_t rfid_read(uint8_t *outBuffer) {
    uint8_t count = 0;
    uint16_t timeout = 0;
    
    // Lire tous les bytes disponibles
    while (rfid_available() && count < RFID_BUFFER_SIZE) {
        rfid_buffer[count++] = rfid_read_byte();
        
        // Timeout pour éviter un blocage
        timeout = 0;
        while (!rfid_available() && timeout < 1000) {
            _delay_us(10);
            timeout++;
        }
        
        if (timeout >= 1000) break;
    }
    
    // Copier dans le buffer de sortie
    for (uint8_t i = 0; i < count; i++) {
        outBuffer[i] = rfid_buffer[i];
    }
    
    return count;
}

/*
 * Lit uniquement l'ID du tag (10 bytes de données utiles)
 */
bool rfid_read_tag_id(uint8_t *outTagId) {
    uint8_t buffer[RFID_TAG_SIZE];
    uint8_t count = rfid_read(buffer);
    
    // Vérifier si on a reçu un tag complet
    if (count < RFID_TAG_SIZE) {
        return false;
    }
    
    // Vérifier STX et ETX
    if (buffer[0] != RFID_STX || buffer[13] != RFID_ETX) {
        return false;
    }
    
    // Extraire les 10 bytes de données (positions 1 à 10)
    for (uint8_t i = 0; i < RFID_DATA_SIZE; i++) {
        outTagId[i] = buffer[i + 1];
    }
    
    return true;
}

/*
 * Compare deux IDs de tags
 */
bool rfid_compare_tags(const uint8_t *tag1, const uint8_t *tag2) {
    for (uint8_t i = 0; i < RFID_DATA_SIZE; i++) {
        if (tag1[i] != tag2[i]) {
            return false;
        }
    }
    return true;
}

/*
 * Vide le buffer UART
 */
void rfid_clear(void) {
    // Lire tous les bytes disponibles pour vider le buffer
    while (rfid_available()) {
        (void)UDR0;  // Cast void pour éviter warning
        _delay_us(100);
    }
    
    // Réinitialiser le buffer interne
    memset(rfid_buffer, 0, RFID_BUFFER_SIZE);
}

/*
 * Convertit l'ID du tag en chaîne ASCII lisible
 */
void rfid_tag_to_string(const uint8_t *tagId, char *outStr) {
    for (uint8_t i = 0; i < RFID_DATA_SIZE; i++) {
        outStr[i] = (char)tagId[i];
    }
    outStr[RFID_DATA_SIZE] = '\0';  // Terminateur de chaîne
}
