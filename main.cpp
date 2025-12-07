#include "drivers/buzzer/buzzer.h"
#include "drivers/i2c/i2c.h"
#include "drivers/led/led.h"
#include "drivers/rfid/rfid.h"
#include "drivers/lcd/lcd.hpp"
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

// ========================================================================
// CONFIGURATION MATÉRIELLE
// ========================================================================

static constexpr uint8_t RFID_RX_PIN = 2;
static constexpr uint8_t RFID_TX_PIN = 3;

// ========================================================================
// BOUCLE PRINCIPALE (C++)
// ========================================================================

int main(void)
{

    Lcd screen(0x3E, 16, 2);
    screen.init();        // <-- IMPORTANT : appelle init avant toute autre commande
    screen.backlightOn(); // si tu as une méthode pour allumer le rétroéclairage
    _delay_ms(50);        // petit délai entre l'init et les commandes

    screen.clear();

    
    // Tester l'affichage LCD
    screen.setCursor(0, 0);
    screen.print("LabOtrack Ready");

    // Le code n'atteindra jamais ici dans un environnement AVR
    return 0;
}

int main(void) {
    // ========================================================================
    // INITIALISATION
    // ========================================================================

    // Utilisation des gestionnaires statiques
    LedManager::initAll();
    I2CManager::init(); // IMPORTANT : Initialisation de l'I2C avant le LCD

    // Création des instances d'objets (les constructeurs appellent l'init)
    Buzzer systemBuzzer;
    Lcd screen; 

    // Signal de démarrage (LED, Buzzer)
    LedManager::on(LedManager::GREEN);
    systemBuzzer.beep(100);
    _delay_ms(200);
    LedManager::off(LedManager::GREEN);

    // ========================================================================
    // AFFICHAGE INITIAL SUR LE LCD
    // ========================================================================

    screen.clear();

    // Affiche "Hello World!" sur la première ligne
    screen.setCursor(0, 0); 
    screen.print("Hello World!");

    // Affiche "LCD Grove v2.0" sur la deuxième ligne
    screen.setCursor(1, 0); 
    screen.print("LCD Grove v2.0");

    // Signal de succès
    LedManager::on(LedId::GREEN);
    systemBuzzer.patternSuccess();
    LedManager::off(LedId::GREEN);

    _delay_ms(3000);

    // ========================================================================
    // BOUCLE PRINCIPALE
    // ========================================================================
    
    char buffer[Lcd::COLS + 1]; // Buffer pour afficher les chiffres

    while (true) {
        // Exemple 1 : Compteur
        screen.clear();
        screen.setCursor(0, 0);
        screen.print("Compteur:");

        for (uint8_t i = 0; i < 10; i++) {
            // Utilisation de snprintf pour formater le nombre
            snprintf(buffer, Lcd::COLS + 1, "Valeur: %d", i); 

            screen.setCursor(1, 0);
            screen.print(buffer);

            LedManager::on(Led::BLUE);
            systemBuzzer.beep(50);
            LedManager::off(Led::BLUE);

            _delay_ms(1000);
        }

        _delay_ms(1000);

        // Exemple 2 : Messages alternés
        screen.clear();
        screen.setCursor(0, 0);
        screen.print("Lab-O-Track");
        screen.setCursor(1, 0);
        screen.print("Systeme actif");

        LedManager::on(Led::GREEN);
        _delay_ms(2000);
        LedManager::off(Led::GREEN);

        screen.clear();
        screen.setCursor(0, 0);
        screen.print("Pret pour");
        screen.setCursor(1, 0);
        screen.print("tracabilite!");

        LedManager::on(Led::BLUE);
        _delay_ms(2000);
        LedManager::off(Led::BLUE);

        // Exemple 3 : Test du curseur
        screen.clear();
        screen.setCursor(0, 0);
        screen.print("Test curseur");

        screen.setCursor(1, 0);
        screen.cursor(true); // Active le curseur
        screen.print("Visible");
        _delay_ms(2000);

        screen.cursor(false); // Cache le curseur
        screen.blink(true);   // Active le clignotement
        _delay_ms(2000);

        screen.blink(false); // Désactive le clignotement

        _delay_ms(1000);
    }
    
    // return 0; // Inaccessible dans une boucle while(true) AVR
}