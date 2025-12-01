#ifndef BUZZER_H
#define BUZZER_H

#include <avr/io.h>
#include <util/delay.h>

// Configuration : Buzzer sur D6
#define BUZZER_PIN PD6
#define BUZZER_PORT PORTD
#define BUZZER_DDR DDRD

void buzzer_init(void);
void buzzer_on(void);
void buzzer_off(void);
void buzzer_beep(uint16_t duration_ms);
void buzzer_alert_pattern(void);
void buzzer_success_pattern(void);

#endif