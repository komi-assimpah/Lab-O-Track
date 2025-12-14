#include "RFID.h"

RFID::RFID(uint8_t rxPin, uint8_t txPin)
    : softSerial(rxPin, txPin), count(0)
{
}

void RFID::init()
{
    softSerial.begin(9600);
}

bool RFID::available()
{
    return softSerial.available() > 0;
}

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
