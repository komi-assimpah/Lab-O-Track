#ifndef BUZZER_H
#define BUZZER_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define BUZZER_PIN PD7
#define BUZZER_PORT PORTD
#define BUZZER_DDR DDRD

    // Fonctions de base
    void buzzer_init(void);
    void buzzer_on(void);
    void buzzer_off(void);

    // Fonctions de bip
    void buzzer_beep(uint16_t duration_ms);
    void buzzer_beep_blocking(uint16_t duration_ms);

    // Patterns sonores prédéfinis
    void buzzer_pattern_startup(void);
    void buzzer_pattern_alert(void);
    void buzzer_pattern_success(void);
    void buzzer_pattern_error(void);
    void buzzer_pattern_warning(void);

    // Patterns complexes
    void buzzer_pattern_siren(uint8_t cycles);
    void buzzer_pattern_morse_sos(void);

#ifdef __cplusplus
}
#endif

#endif