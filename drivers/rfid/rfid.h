#ifndef RFID_READER_H
#define RFID_READER_H

#include <stdint.h>
#include <stdbool.h>
#include <SoftwareSerial.h> 

/**
 * @brief Classe pour gérer la communication avec un lecteur RFID via SoftwareSerial.
 */
class RfidReader {
public:
    // Taille maximale du buffer pour une trame RFID
    static constexpr uint8_t BUFFER_SIZE = 64;

    /**
     * @brief Constructeur. Initialise la communication série logicielle.
     * @param rxPin La broche de réception (Rx) de l'Arduino.
     * @param txPin La broche d'émission (Tx) de l'Arduino.
     * @param baud Taux de transmission (par défaut 9600).
     */
    RfidReader(uint8_t rxPin, uint8_t txPin, long baud = 9600);

    /**
     * @brief Vérifie si des données sont disponibles à lire sur le port série.
     * @return true si des octets sont en attente, false sinon.
     */
    bool available();

    /**
     * @brief Lit tous les octets disponibles du port série et les stocke dans un buffer externe.
     *
     * @param outBuffer Le buffer de destination où copier les données lues.
     * @return Le nombre d'octets lus.
     */
    uint8_t read(uint8_t *outBuffer);

    /**
     * @brief Vide le buffer interne de lecture.
     */
    void clear();

private:
    // Instance de SoftwareSerial gérée par cette classe
    SoftwareSerial serial; 

    // Buffer interne de lecture
    uint8_t rfid_buffer[BUFFER_SIZE];
    uint8_t current_count = 0;
};

#endif // RFID_READER_H