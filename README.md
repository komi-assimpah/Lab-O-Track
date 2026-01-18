# Lab-O-Track

Système de surveillance d'équipements de laboratoire avec alarme locale (Arduino) et notifications distantes (Discord).

---

## Matériel Requis

**Arduino (Nœud embarqué) :**
- Arduino Uno
- Grove Base Shield V2
- Grove - 125KHz RFID Reader
- Tag RFID 125KHz
- 3x Grove - LED (Rouge, Verte, Bleue)
- Grove - Buzzer

**Raspberry Pi (Passerelle) :**
- Raspberry Pi 3 (ou supérieur)
- GrovePi+
- Connexion Internet (pour notifications Discord)

**Câblage :**
- Câbles Grove (fournis avec les modules)
- 1x câble Grove 4 pins pour connexion I2C entre shields


---

## Câblage

### Arduino Uno (avec Grove Base Shield)

Le **Grove Base Shield** est monté sur l'Arduino. Les composants se connectent via les ports Grove :

| Composant | Port Grove |
|-----------|------------|
| **Grove RFID Reader** | D2 |
| **LED Rouge** | D4 |
| **LED Verte** | D5 |
| **LED Bleue** | D6 |
| **Buzzer** | D7 |

### Raspberry Pi 3 (avec GrovePi+)

Le **GrovePi+** est monté sur la Raspberry Pi.

### Communication I2C (Arduino ↔ Raspberry Pi)

Utilisez un **câble Grove 4 pins** pour connecter directement les ports I2C des deux shields :
- Grove Base Shield : Port I2C
- GrovePi+ : Port I2C


---

## Installation

### Arduino

```bash
# Installer la toolchain AVR
sudo apt-get install gcc-avr avr-libc binutils-avr avrdude

# Compiler et téléverser
make upload

```

### Raspberry Pi

```bash
# Activer I2C
sudo raspi-config  # Interface Options > I2C > Enable

# Installer dépendances Python
sudo apt-get install i2c-tools
pip3 install smbus2 requests

# Vérifier détection Arduino (doit afficher 42)
i2cdetect -y 1
```

---

## Configuration

Éditez `rpi/data/arduinos_config.json` :

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
        "discord_webhook": "https://discord.com/api/webhooks/VOTRE_WEBHOOK_ICI"
    }
}
```

**Paramètres clés :**
- `i2c_address` : Adresse I2C de l'Arduino (66 = 0x42 en décimal)
- `timeout_seconds` : Délai avant alarme (en secondes)
- `allowed_tags` : Liste des tags RFID autorisés
- `discord_webhook` : URL du webhook Discord pour les notifications

---

## Utilisation

Lancer la passerelle :

```bash
cd rpi
python3 main.py
```

**Comportement du système :**
1. **Objet rangé** : LED Verte
2. **Objet emprunté** : LED Bleue, timer démarre
3. **Temps dépassé** : LED Rouge + Buzzer + Notification Discord (alarme démarrée)
4. **Objet retourné** : LED Verte + Arrêt alarme + Notification Discord (alarme arrêtée)

---

## Auteurs

- **Sagesse Kokouvi ADABADJI**
- **Momen TAKROUN**
- **Komi Jean Paul ASSIMPAH**
