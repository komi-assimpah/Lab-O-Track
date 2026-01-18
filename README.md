# Lab-O-Track

Laboratory equipment monitoring system with local alarm (Arduino) and remote notifications (Discord).

---

## Required Hardware

**Arduino (Embedded Node):**
- Arduino Uno
- Grove Base Shield V2
- Grove - 125KHz RFID Reader
- 125KHz RFID Tag
- 3x Grove - LED (Red, Green, Blue)
- Grove - Buzzer

**Raspberry Pi (Gateway):**
- Raspberry Pi 3 (or higher)
- GrovePi+
- Internet connection (for Discord notifications)

**Wiring:**
- Grove cables (included with modules)
- 1x Grove 4-pin cable for I2C connection between shields

---

## Wiring

### Arduino Uno (with Grove Base Shield)

The **Grove Base Shield** is mounted on the Arduino. Components connect via Grove ports:

| Component | Grove Port |
|-----------|------------|
| **Grove RFID Reader** | D2 |
| **Red LED** | D4 |
| **Green LED** | D5 |
| **Blue LED** | D6 |
| **Buzzer** | D7 |

### Raspberry Pi 3 (with GrovePi+)

The **GrovePi+** is mounted on the Raspberry Pi.

### I2C Communication (Arduino ↔ Raspberry Pi)

Use a **Grove 4-pin cable** to directly connect the I2C ports of both shields:
- Grove Base Shield: I2C Port
- GrovePi+: I2C Port

---

## Installation

### Arduino

```bash
# Install AVR toolchain
sudo apt-get install gcc-avr avr-libc binutils-avr avrdude

# Compile Arduino libraries
cd src/lib/arduinoLibsAndCore
make
cd ../../..

# Compile and upload firmware
cd src
make upload
```

### Raspberry Pi

```bash
# Enable I2C
sudo raspi-config  # Interface Options > I2C > Enable

# Install Python dependencies
sudo apt-get install i2c-tools
pip3 install smbus2 requests

# Verify Arduino detection (should display 42)
i2cdetect -y 1
```

---

## Configuration

Edit `rpi/data/arduinos_config.json`:

```json
{
    "devices": [
        {
            "id": 1,
            "name": "Oscilloscope 01",
            "i2c_address": 66,
            "timeout_seconds": 3600,
            "allowed_tags": ["0123456789AB"]
        }
    ],
    "alerts": {
        "discord_webhook": "https://discord.com/api/webhooks/YOUR_WEBHOOK_HERE"
    }
}
```

**Key parameters:**
- `i2c_address`: Arduino I2C address (66 = 0x42 in decimal)
- `timeout_seconds`: Delay before alarm (in seconds)
- `allowed_tags`: List of authorized RFID tags
- `discord_webhook`: Discord webhook URL for notifications

---

## Usage

Launch the gateway:

```bash
cd rpi
python3 main.py
```

**System behavior:**
1. **Item stored**: Green LED
2. **Item borrowed**: Blue LED, timer starts
3. **Timeout exceeded**: Red LED + Buzzer + Discord notification (alarm started)
4. **Item returned**: Green LED + Alarm stopped + Discord notification (alarm stopped)

---

## Authors

- **Sagesse Kokouvi ADABADJI**
- **Momen TAKROUN**
- **Komi Jean Paul ASSIMPAH**
