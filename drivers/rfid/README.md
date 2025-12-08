# 📡 Driver RFID - Grove 125KHz Reader

Documentation complète du driver RFID pour le projet Lab-O-Track.

## 📋 Spécifications Matériel

- **Module** : Grove 125KHz RFID Reader
- **Protocole** : UART (série)
- **Baud Rate** : 9600
- **Pins** : 
  - RX (Arduino D0 / ATmega328P PD0)
  - TX (Arduino D1 / ATmega328P PD1)
- **Tags supportés** : RFID 125KHz (EM4100, compatibles)

## ⚠️ IMPORTANT : Conflit avec Serial Monitor

Le lecteur RFID utilise les pins RX/TX (D0/D1) qui sont **les mêmes** que le port USB série.

**Conséquences** :
- ❌ Vous **NE POUVEZ PAS** utiliser `Serial.print()` et le RFID simultanément
- ❌ Le Serial Monitor interfère avec le RFID
- ✅ **Solution** : Débranchez le câble USB après l'upload pour tester le RFID

## 📦 Format des Données RFID

Le lecteur envoie **14 bytes** par tag détecté :

```
Byte 0    : STX (0x02) - Start of Text
Bytes 1-10: ID du tag (10 caractères ASCII)
Bytes 11-12: Checksum (2 bytes)
Byte 13   : ETX (0x03) - End of Text
```

### Exemple de tag :
```
0x02 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x30 0xXX 0xXX 0x03
     '1'  '2'  '3'  '4'  '5'  '6'  '7'  '8'  '9'  '0'  checksum
```

## 🔧 API du Driver

### Initialisation
```cpp
void rfid_init(void);
```
Configure l'UART à 9600 bauds pour communiquer avec le lecteur.

### Vérification de disponibilité
```cpp
bool rfid_available(void);
```
Retourne `true` si des données RFID sont disponibles.

### Lecture complète (14 bytes)
```cpp
uint8_t rfid_read(uint8_t *outBuffer);
```
Lit tous les bytes disponibles (incluant STX, ETX, checksum).  
**Retourne** : Nombre de bytes lus.

### Lecture de l'ID seulement (10 bytes)
```cpp
bool rfid_read_tag_id(uint8_t *outTagId);
```
Lit et extrait uniquement l'ID du tag (sans STX/ETX/checksum).  
**Retourne** : `true` si succès, `false` si erreur.

### Comparaison de tags
```cpp
bool rfid_compare_tags(const uint8_t *tag1, const uint8_t *tag2);
```
Compare deux IDs de tags (10 bytes chacun).  
**Retourne** : `true` si identiques.

### Conversion en chaîne
```cpp
void rfid_tag_to_string(const uint8_t *tagId, char *outStr);
```
Convertit un ID de tag en chaîne ASCII lisible (11 bytes avec '\0').

### Nettoyage du buffer
```cpp
void rfid_clear(void);
```
Vide le buffer UART pour supprimer les données résiduelles.

## 💻 Exemple d'Utilisation Simple

```cpp
#include "drivers/rfid/rfid.h"
#include "drivers/lcd/lcd.h"
#include "drivers/led/led.h"

int main(void) {
    // Initialisation
    rfid_init();
    lcd_init();
    led_init_all();
    
    uint8_t tag_id[RFID_DATA_SIZE];
    char tag_string[RFID_DATA_SIZE + 1];
    
    lcd_clear();
    lcd_print("En attente...");
    
    while (1) {
        if (rfid_available()) {
            if (rfid_read_tag_id(tag_id)) {
                // Tag lu avec succès
                rfid_tag_to_string(tag_id, tag_string);
                
                lcd_clear();
                lcd_set_cursor(0, 0);
                lcd_print("Tag detecte:");
                lcd_set_cursor(1, 0);
                lcd_print(tag_string);
                
                led_on(LED_GREEN);
                _delay_ms(2000);
                led_off(LED_GREEN);
                
                rfid_clear();
            }
        }
        _delay_ms(100);
    }
}
```

## 🔍 Exemple : Système d'Accès

```cpp
// Tag autorisé (à définir après première lecture)
const uint8_t AUTHORIZED_TAG[10] = {'1','2','3','4','5','6','7','8','9','0'};

uint8_t scanned_tag[RFID_DATA_SIZE];

if (rfid_available()) {
    if (rfid_read_tag_id(scanned_tag)) {
        if (rfid_compare_tags(AUTHORIZED_TAG, scanned_tag)) {
            // Accès autorisé
            lcd_print("ACCES OK");
            led_on(LED_GREEN);
            buzzer_pattern_success();
        } else {
            // Accès refusé
            lcd_print("ACCES REFUSE");
            led_on(LED_RED);
            buzzer_pattern_error();
        }
        rfid_clear();
    }
}
```

## 🧪 Test du Driver

Compilez et uploadez le test :
```bash
cd tests
make test_rfid
```

**Procédure de test :**
1. Compilez et uploadez le programme
2. **DÉBRANCHEZ** le câble USB
3. Alimentez l'Arduino avec une source externe (batterie, adaptateur)
4. Approchez un tag RFID du lecteur
5. Observez les LEDs et le LCD

## 🐛 Dépannage

### Le lecteur ne détecte rien
- ✅ Vérifiez les connexions RX/TX
- ✅ Câble USB débranché ?
- ✅ Alimentation externe branchée ?
- ✅ Tag RFID compatible 125KHz ?

### Données incorrectes
- ✅ Utilisez `rfid_clear()` entre chaque lecture
- ✅ Ajoutez un délai après chaque lecture
- ✅ Vérifiez STX (0x02) et ETX (0x03)

### Conflit avec upload
- ✅ Le RFID doit être **débranché** pendant l'upload
- ✅ Ou bien désactivez temporairement `rfid_init()`

## 📊 Différences avec le Code Original

| Aspect | Code Original | Notre Driver |
|--------|---------------|--------------|
| **Bibliothèque** | SoftwareSerial (Arduino) | UART matériel AVR |
| **Pins** | D2, D3 (configurables) | D0, D1 (UART fixe) |
| **Dépendances** | Framework Arduino | AVR pur (libc) |
| **Performance** | Émulation logicielle | Matériel (plus rapide) |
| **Fiabilité** | Peut perdre des bytes | Fiable avec buffer UART |

## 🎯 Intégration dans Lab-O-Track

Le driver RFID sera utilisé pour :
1. **Identifier les équipements** via tags RFID uniques
2. **Détecter les absences** (tag non présent)
3. **Déclencher les timers** quand un équipement sort de sa zone
4. **Valider les retours** d'équipements

Exemple : 
- Tag "OSC-01" détecté → Équipement présent
- Tag "OSC-01" absent → Timer démarre (15 min)
- Tag "OSC-01" réapparaît → Timer annulé

## 📝 Notes Techniques

- **Buffer UART** : 64 bytes (configurable via `RFID_BUFFER_SIZE`)
- **Timeout** : 1000 × 10µs = 10ms entre bytes
- **Format tags** : Compatible EM4100 et similaires
- **Portée** : ~10cm (dépend du lecteur et du tag)
