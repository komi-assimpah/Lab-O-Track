#ifndef BUTTON_H
#define BUTTON_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

// Configuration : Bouton sur D7
#define BUTTON_PIN PD7
#define BUTTON_PORT PORTD
#define BUTTON_DDR DDRD
#define BUTTON_PINREG PIND

// Debounce delay
#define DEBOUNCE_DELAY_MS 50

void button_init(void);
bool button_is_pressed(void);
bool button_wait_press(void);
bool button_wait_release(void);

#endif