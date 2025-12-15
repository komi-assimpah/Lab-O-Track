/*
 * ============================================================================
 * PROTOCOLE DE COMMUNICATION I2C - LAB-O-TRACK
 * ============================================================================
 *
 * Ce fichier définit le protocole de communication entre le Raspberry Pi
 * (maître I2C) et l'Arduino (esclave I2C) pour le système de traçabilité.
 *
 * Architecture :
 *   - Raspberry Pi : Maître I2C, poll régulièrement l'Arduino
 *   - Arduino : Esclave I2C à l'adresse 0x08, répond aux requêtes
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define I2C_SLAVE_ADDRESS 0x08

// ============================================================================
// COMMANDES (RPi -> Arduino)
// ============================================================================
#define CMD_GET_STATUS  0x01
#define CMD_GET_TAG     0x02  // Obtenir le tag RFID (ASCII)
#define CMD_GET_TIMER   0x03


// ============================================================================
// CODES DE STATUT (Arduino -> RPi)
// ============================================================================
#define STATUS_OK           0x00  // Équipement en place, tout va bien
#define STATUS_TAG_PRESENT  0x01  // Tag détecté, en cours d'utilisation
#define STATUS_TAG_MISSING  0x02  // Tag absent, timer en cours
#define STATUS_ALERT        0x03  // ALERTE ! Timer expiré



// ============================================================================
// FORMAT DES DONNÉES
// ============================================================================
#define TAG_ASCII_SIZE     10
#define TAG_BUFFER_SIZE    11  // +1 pour le '\0'


typedef struct {
    uint8_t status;                    
    char    tag_id[TAG_BUFFER_SIZE];
    uint16_t timer_remaining;
} LabTrackData;




// Durées par défaut avant alerte (en secondes)
#define TIMER_DEFAULT      (30 * 60)
#define TIMER_OSCILLOSCOPE (30 * 60)
#define TIMER_MULTIMETER   (15 * 60)
#define TIMER_LAPTOP       (60 * 60)

#ifdef __cplusplus
}
#endif

#endif // PROTOCOL_H
