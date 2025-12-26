#include "led.h"

void led_init_all(void)
{
    LED_DDR |= (1 << LED_RED_PIN) | (1 << LED_GREEN_PIN) | (1 << LED_BLUE_PIN);
    LED_BUILTIN_DDR |= (1 << LED_BUILTIN_PIN);

    led_all_off();
}

// Allumer une LED spécifique
void led_on(led_id_t led)
{
    switch (led)
    {
    case LED_RED:
        LED_PORT |= (1 << LED_RED_PIN);
        break;
    case LED_GREEN:
        LED_PORT |= (1 << LED_GREEN_PIN);
        break;
    case LED_BLUE:
        LED_PORT |= (1 << LED_BLUE_PIN);
        break;
    case LED_BUILTIN_IN:
        LED_BUILTIN_PORT |= (1 << LED_BUILTIN_PIN);
        break;
    }
}

// Éteindre une LED spécifique
void led_off(led_id_t led)
{
    switch (led)
    {
    case LED_RED:
        LED_PORT &= ~(1 << LED_RED_PIN);
        break;
    case LED_GREEN:
        LED_PORT &= ~(1 << LED_GREEN_PIN);
        break;
    case LED_BLUE:
        LED_PORT &= ~(1 << LED_BLUE_PIN);
        break;
    case LED_BUILTIN_IN:
        LED_BUILTIN_PORT &= ~(1 << LED_BUILTIN_PIN);
        break;
    }
}

// Inverser l'état d'une LED
void led_toggle(led_id_t led)
{
    switch (led)
    {
    case LED_RED:
        LED_PORT ^= (1 << LED_RED_PIN);
        break;
    case LED_GREEN:
        LED_PORT ^= (1 << LED_GREEN_PIN);
        break;
    case LED_BLUE:
        LED_PORT ^= (1 << LED_BLUE_PIN);
        break;
    case LED_BUILTIN_IN:
        LED_BUILTIN_PORT ^= (1 << LED_BUILTIN_PIN);
        break;
    }
}

// Faire clignoter une LED
void led_blink(led_id_t led, uint16_t duration_ms)
{
    led_on(led);
    for (uint16_t i = 0; i < duration_ms; i++)
    {
        _delay_ms(1);
    }
    led_off(led);
}

// Allumer toutes les LEDs
void led_all_on(void)
{
    led_on(LED_RED);
    led_on(LED_GREEN);
    led_on(LED_BLUE);
    led_on(LED_BUILTIN_IN);
}

// Éteindre toutes les LEDs
void led_all_off(void)
{
    led_off(LED_RED);
    led_off(LED_GREEN);
    led_off(LED_BLUE);
    led_off(LED_BUILTIN_IN);
}

// Pattern de démarrage
void led_pattern_startup(void)
{
    led_all_off();

    // Séquence rouge -> vert -> bleu
    led_blink(LED_RED, 200);
    _delay_ms(50);
    led_blink(LED_GREEN, 200);
    _delay_ms(50);
    led_blink(LED_BLUE, 200);
    _delay_ms(50);

    // Toutes ensemble
    led_all_on();
    _delay_ms(300);
    led_all_off();
}

// Pattern d'alerte (rouge clignotant)
void led_pattern_alert(void)
{
    led_all_off();

    for (uint8_t i = 0; i < 3; i++)
    {
        led_on(LED_RED);
        led_on(LED_BUILTIN_IN);
        _delay_ms(150);
        led_off(LED_RED);
        led_off(LED_BUILTIN_IN);
        _delay_ms(150);
    }
}

// Pattern de succès (vert clignotant rapide)
void led_pattern_success(void)
{
    led_all_off();

    for (uint8_t i = 0; i < 2; i++)
    {
        led_on(LED_GREEN);
        _delay_ms(80);
        led_off(LED_GREEN);
        _delay_ms(80);
    }
}

// Pattern séquence (pour test)
void led_pattern_sequence(void)
{
    led_all_off();

    // Rouge
    led_on(LED_RED);
    led_on(LED_BUILTIN_IN);
    _delay_ms(500);

    // Vert
    led_all_off();
    led_on(LED_GREEN);
    led_on(LED_BUILTIN_IN);
    _delay_ms(500);

    // Bleu
    led_all_off();
    led_on(LED_BLUE);
    led_on(LED_BUILTIN_IN);
    _delay_ms(500);

    // Toutes
    led_all_on();
    _delay_ms(500);

    // Éteintes
    led_all_off();
    _delay_ms(500);

    // Clignotement rapide
    for (uint8_t i = 0; i < 5; i++)
    {
        led_on(LED_RED);
        _delay_ms(100);
        led_off(LED_RED);

        led_on(LED_GREEN);
        _delay_ms(100);
        led_off(LED_GREEN);

        led_on(LED_BLUE);
        _delay_ms(100);
        led_off(LED_BLUE);
    }
}