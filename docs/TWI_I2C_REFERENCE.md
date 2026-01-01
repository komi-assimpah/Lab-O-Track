# Documentation TWI (I2C) - ATMega328P

## 📖 Source Officielle
ATMega328P Datasheet (Microchip), Chapitre 21 "2-wire Serial Interface (TWI)"

---

## Les 4 Registres TWI

| Registre | Rôle |
|----------|------|
| TWAR | Notre adresse en mode Slave |
| TWCR | Contrôle du module (on/off, ACK, interrupt) |
| TWSR | Status - ce qui vient de se passer |
| TWDR | Données à envoyer/reçues |

---

## TWAR - Adresse Slave

```
Bit:     7      6      5      4      3      2      1      0
       ┌──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┐
TWAR:  │ TWA6 │ TWA5 │ TWA4 │ TWA3 │ TWA2 │ TWA1 │ TWA0 │TWGCE │
       └──────┴──────┴──────┴──────┴──────┴──────┴──────┴──────┘
```

Bits 7:1 = Adresse 7 bits, Bit 0 = General Call Enable

```c
TWAR = (0x42 << 1);  // Adresse 0x42, décalée car bit 0 réservé
```

---

## TWCR - Contrôle

```
Bit:     7      6      5      4      3      2      1      0
       ┌──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┐
TWCR:  │TWINT │ TWEA │TWSTA │TWSTO │ TWWC │ TWEN │  -   │ TWIE │
       └──────┴──────┴──────┴──────┴──────┴──────┴──────┴──────┘
```

| Bit | Nom | Description |
|-----|-----|-------------|
| TWINT | Interrupt Flag | Mis à 1 quand opération terminée |
| TWEA | Enable ACK | Répondre ACK automatiquement |
| TWEN | Enable | Activer le module TWI |
| TWIE | Interrupt Enable | Activer l'interruption |

```c
TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
```

---

## TWSR - Status (toujours masquer avec 0xF8)

```c
uint8_t status = TWSR & 0xF8;
```

### Codes Slave Receiver (Master nous écrit)

| Code | Signification |
|------|---------------|
| 0x60 | Adresse + W reçue, ACK envoyé |
| 0x80 | Donnée reçue, ACK envoyé |
| 0xA0 | STOP reçu |

### Codes Slave Transmitter (Master nous lit)

| Code | Signification |
|------|---------------|
| 0xA8 | Adresse + R reçue, ACK envoyé |
| 0xB8 | Donnée transmise, ACK reçu |
| 0xC0 | Donnée transmise, NACK reçu (fin) |

---

## TWDR - Données

Lire : `uint8_t data = TWDR;`
Écrire : `TWDR = data_to_send;`
