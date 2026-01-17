#include "rfid.h"

RFID::RFID(uint8_t rxPin, uint8_t txPin)
    : softSerial(rxPin, txPin), count(0)
{
}

void RFID::init()
{
    // Initialize SoftwareSerial for RFID communication
    softSerial.begin(9600);
}

bool RFID::available()
{
    // Check if tag data is available
    return softSerial.available() > 0;
}

// Read data from RFID tag into buffer
int RFID::read()
{
    while (softSerial.available())
    {
        buffer[count++] = softSerial.read();
        if (count >= RFID_BUFFER_SIZE)
            break;
    }
    return count;
}

unsigned char *RFID::get_buffer()
{
    return buffer;
}

void RFID::clear()
{
    for (int i = 0; i < count; i++)
        buffer[i] = 0;
    count = 0;
}
