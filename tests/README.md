# Tests des Drivers Lab-O-Track

Ce dossier contient les fichiers de test pour tous les drivers du système Lab-O-Track.

## 📋 Liste des Tests

### 1. **test_led.cpp**
- Test du driver LED (Rouge, Verte, Bleue, Builtin)
- Fonctions testées : ON/OFF, Toggle, Blink, Patterns

### 2. **test_buzzer.cpp**
- Test du driver Buzzer
- Fonctions testées : Beep simple, Patterns sonores, Siren, Morse SOS

### 3. **test_button.cpp**
- Test du driver Button
- Fonctions testées : Détection d'appui, Wait press/release, Compteur

### 4. **test_i2c.cpp**
- Test du driver I2C
- Fonctions testées : Scan du bus, Communication, Statut

### 5. **test_lcd.cpp**
- Test du driver LCD Grove 16x2
- Fonctions testées : Affichage texte, Positionnement, Clear, Compteur
- **⚠️ NÉCESSITE UN LCD CONNECTÉ**

## 🔧 Compilation et Upload

### Compiler et uploader un test spécifique :
```bash
cd tests
make test_led       # Upload test LED
make test_buzzer    # Upload test Buzzer
make test_button    # Upload test Button
make test_i2c       # Upload test I2C
make test_lcd       # Upload test LCD
```

### Compiler tous les tests (sans upload) :
```bash
make all_tests
```

### Nettoyer les fichiers compilés :
```bash
make clean
```

### Afficher l'aide :
```bash
make info
```

## 🔌 Matériel Requis

| Test | Matériel Nécessaire |
|------|---------------------|
| **LED** | 3 LEDs sur D2 (Rouge), D3 (Verte), D4 (Bleue) + LED builtin |
| **Buzzer** | Buzzer sur D6 + LEDs |
| **Button** | Bouton sur D7 + LEDs + Buzzer |
| **I2C** | Périphérique I2C (ex: LCD) sur A4/A5 + LEDs |
| **LCD** | LCD Grove 16x2 sur I2C + LEDs + Buzzer |

## 📊 Interprétation des Résultats

### Signaux Visuels (LEDs)
- 🟢 **Vert** : Succès / En cours / Actif
- 🔴 **Rouge** : Erreur / Alerte / En attente
- 🔵 **Bleu** : Information / Traitement
- 💡 **Builtin** : Debug / Activité

### Signaux Sonores (Buzzer)
- **Bip court** : Confirmation
- **Bip long** : Succès
- **Bips répétés** : Alerte/Erreur
- **Pattern mélodique** : Startup/Success

## ⚙️ Configuration du Port

Si votre Arduino n'est pas sur `/dev/ttyACM0`, modifiez le Makefile :
```makefile
PORT = /dev/ttyUSB0  # ou votre port
```

## 🐛 Dépannage

### Erreur de compilation
```bash
make clean
make test_led  # Réessayer
```

### Erreur d'upload
1. Vérifier que l'Arduino est connecté : `ls /dev/tty*`
2. Vérifier les permissions : `sudo usermod -a -G dialout $USER`
3. Se reconnecter ou utiliser : `sudo make test_led`

### Test LCD ne fonctionne pas
1. Vérifier connexions I2C (SDA=A4, SCL=A5)
2. Vérifier alimentation du LCD
3. Tester avec `make test_i2c` pour vérifier la détection

## 📝 Notes

- Tous les tests se terminent par une boucle infinie avec feedback visuel
- Appuyez sur le bouton RESET pour arrêter un test
- Les tests utilisent des délais pour observer les résultats
- Certains tests nécessitent une interaction (ex: test_button)

## 🚀 Ordre de Test Recommandé

1. **test_led** - Vérifier que toutes les LEDs fonctionnent
2. **test_buzzer** - Vérifier le buzzer
3. **test_button** - Vérifier le bouton (interactif)
4. **test_i2c** - Scanner le bus I2C
5. **test_lcd** - Tester l'affichage LCD
