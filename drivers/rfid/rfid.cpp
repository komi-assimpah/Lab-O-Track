#include "rfid.h"
#include <string.h> // Pour memcpy ou memset si nécessaire, bien que les boucles suffisent ici.

/**
 * @brief Constructeur. Initialise la communication série logicielle. (Remplace rfid_init)
 */
RfidReader::RfidReader(uint8_t rxPin, uint8_t txPin, long baud) 
    : serial(rxPin, txPin) // Initialise l'objet SoftwareSerial avec les broches
{
    // Initialise la communication SoftwareSerial
    serial.begin(baud);
    
    // Initialiser les membres internes
    current_count = 0;
    clear(); 
}

/**
 * @brief Vérifie si des données sont disponibles. (Remplace rfid_available)
 */
bool RfidReader::available() {
    return serial.available();
}

/**
 * @brief Vide le buffer interne. (Remplace rfid_clear)
 */
void RfidReader::clear() {
    // memset est plus idiomatique en C++ pour effacer un tableau
    memset(rfid_buffer, 0, BUFFER_SIZE);
    current_count = 0;
}

/**
 * @brief Lit tous les octets disponibles. (Remplace rfid_read)
 */
uint8_t RfidReader::read(uint8_t *outBuffer) {
    current_count = 0;

    // Lecture de tous les octets disponibles jusqu'à ce que le buffer soit plein
    while (serial.available()) {
        // Lecture et stockage dans le buffer interne
        rfid_buffer[current_count++] = serial.read(); 
        
        if (current_count == BUFFER_SIZE) {
            break;
        }
    }

    // Copie du contenu du buffer interne vers le buffer externe de l'utilisateur
    for (uint8_t i = 0; i < current_count; i++) {
        outBuffer[i] = rfid_buffer[i];
    }
    
    // Vider le buffer interne après lecture
    clear(); 
    
    // Retourner le nombre d'octets lus
    return current_count;
}