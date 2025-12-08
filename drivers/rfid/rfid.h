#ifndef RFID_H
#define RFID_H

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ============================================================================
 * DRIVER RFID GROVE 125KHz READER
 * ============================================================================
 *
 * Ce driver contrôle le lecteur RFID Grove 125KHz via UART.
 * 
 * Matériel :
 *   - Grove - 125KHz RFID Reader
 *   - Connexion UART : RX = PD0 (Pin 0), TX = PD1 (Pin 1)
 *   - Baud rate : 9600
 * 
 * Format des données RFID :
 *   - Le lecteur envoie 14 bytes quand un tag est détecté
 *   - Format : STX (0x02) + 10 bytes data + 2 bytes checksum + ETX (0x03)
 *   - Exemple : 0x02 0x31 0x32 ... 0xXX 0xXX 0x03
 */

#define RFID_BUFFER_SIZE 64
#define RFID_TAG_SIZE 14        // Taille complète d'un tag RFID
#define RFID_DATA_SIZE 10       // Taille des données utiles (sans STX/ETX/checksum)
#define RFID_STX 0x02          // Start of Text
#define RFID_ETX 0x03          // End of Text

/**
 * Initialise le module RFID via UART
 * 
 * Configure l'UART à 9600 bauds pour communiquer avec le lecteur RFID
 */
void rfid_init(void);

/**
 * Vérifie si des données RFID sont disponibles
 * 
 * Retourne : true si des données sont disponibles, false sinon
 */
bool rfid_available(void);

/**
 * Lit un byte depuis le lecteur RFID
 * 
 * Retourne : le byte lu
 */
uint8_t rfid_read_byte(void);

/**
 * Lit un tag RFID complet
 * 
 * Paramètres :
 *   - outBuffer : buffer pour stocker les données lues (min 14 bytes)
 * 
 * Retourne : nombre de bytes lus, 0 si aucun tag détecté
 */
uint8_t rfid_read(uint8_t *outBuffer);

/**
 * Lit uniquement les données utiles du tag (sans STX/ETX/checksum)
 * 
 * Paramètres :
 *   - outTagId : buffer pour stocker l'ID du tag (min 10 bytes)
 * 
 * Retourne : true si tag lu avec succès, false sinon
 */
bool rfid_read_tag_id(uint8_t *outTagId);

/**
 * Compare deux IDs de tags
 * 
 * Paramètres :
 *   - tag1 : premier tag (10 bytes)
 *   - tag2 : deuxième tag (10 bytes)
 * 
 * Retourne : true si identiques, false sinon
 */
bool rfid_compare_tags(const uint8_t *tag1, const uint8_t *tag2);

/**
 * Vide le buffer UART
 */
void rfid_clear(void);

/**
 * Convertit l'ID du tag en chaîne ASCII
 * 
 * Paramètres :
 *   - tagId : l'ID du tag (10 bytes)
 *   - outStr : buffer pour la chaîne (min 11 bytes pour '\0')
 */
void rfid_tag_to_string(const uint8_t *tagId, char *outStr);

#ifdef __cplusplus
}
#endif

#endif // RFID_H
