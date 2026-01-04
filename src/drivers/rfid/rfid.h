#ifndef RFID_H
#define RFID_H

#include <SoftwareSerial.h>

#define RFID_BUFFER_SIZE 16

class RFID
{
public:
    RFID(uint8_t rxPin, uint8_t txPin);

    void init();
    bool available();
    int read();
    unsigned char *get_buffer();
    void clear();

private:
    SoftwareSerial softSerial;
    unsigned char buffer[RFID_BUFFER_SIZE];
    uint8_t count;
};

#endif
