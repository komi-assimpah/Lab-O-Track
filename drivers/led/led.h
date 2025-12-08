#ifndef LED_H
#define LED_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Configuration des pins pour les 3 LEDs externes
#define LED_RED_PIN     PD2
#define LED_GREEN_PIN   PD3
#define LED_BLUE_PIN    PD4

#define LED_PORT        PORTD
#define LED_DDR         DDRD

// LED intégrée (Pin 13) pour debug
#define LED_BUILTIN_PIN PB5
#define LED_BUILTIN_DDR DDRB
#define LED_BUILTIN_PORT PORTB

// Type énuméré pour identifier les LEDs
typedef enum {
    LED_RED,
    LED_GREEN,
    LED_BLUE,
    LED_BUILTIN
} led_id_t;

// Fonctions d'initialisation
void led_init_all(void);

// Contrôle individuel des LEDs
void led_on(led_id_t led);
void led_off(led_id_t led);
void led_toggle(led_id_t led);
void led_blink(led_id_t led, uint16_t duration_ms);

// Contrôle groupé
void led_all_on(void);
void led_all_off(void);

// Patterns prédéfinis
void led_pattern_startup(void);
void led_pattern_alert(void);
void led_pattern_success(void);
void led_pattern_sequence(void);

#ifdef __cplusplus
}
#endif

#endif