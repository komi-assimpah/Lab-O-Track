/*
 * FreeRTOS Configuration for Lab-O-Track (ATMega328P / Arduino Uno)
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <avr/io.h>

/* ============================================================================
 * CONFIGURATION PROJET LAB-O-TRACK
 * ============================================================================ */

/* Pins RFID */
#define RFID_RX_PIN 2
#define RFID_TX_PIN 3

#define TAG_TARGET "OSC-01"

#define SECURITY_TIMEOUT_MS 6000
// #define SECURITY_TIMEOUT_MS (1UL * 60UL * 1000UL)
//#define SECURITY_TIMEOUT_MS 60000

/* Priorités des tâches */
#define TASK_SENSOR_PRIORITY (tskIDLE_PRIORITY + 3)  /* High */
#define TASK_LOGIC_PRIORITY  (tskIDLE_PRIORITY + 2)  /* Middle */
#define TASK_ALARM_PRIORITY  (tskIDLE_PRIORITY + 1)  /* Low */

/* ============================================================================
 * CONFIGURATION FREERTOS POUR ATMEGA328P
 * ============================================================================ */

#define configCPU_CLOCK_HZ          ((unsigned long)16000000) /* Fréquence CPU (Arduino Uno = 16MHz) */
#define configTICK_RATE_HZ          ((TickType_t)100) /* Fréquence des ticks (100 Hz = tick toutes les 10ms) */
#define configUSE_PREEMPTION        1
#define configUSE_16_BIT_TICKS      1
#define configMAX_PRIORITIES        5
#define configMINIMAL_STACK_SIZE    70 /*(en mots, pas en bytes!)  */
#define configMAX_TASK_NAME_LEN     8

/* La tâche Idle peut céder le CPU */
#define configIDLE_SHOULD_YIELD     1

/* ============================================================================
 * TIMERS SOFTWARE
 * ============================================================================ */

#define configUSE_TIMERS                1
#define configTIMER_TASK_PRIORITY       (configMAX_PRIORITIES - 1)
#define configTIMER_TASK_STACK_DEPTH    configMINIMAL_STACK_SIZE
#define configTIMER_QUEUE_LENGTH        3

/* ============================================================================
 * ALLOCATION MÉMOIRE
 * ============================================================================ */

#define configSUPPORT_STATIC_ALLOCATION     0
#define configSUPPORT_DYNAMIC_ALLOCATION    1
#define configTOTAL_HEAP_SIZE               800

/* ============================================================================
 * HOOKS (désactivés pour économiser la RAM)
 * ============================================================================ */

#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0
#define configUSE_MALLOC_FAILED_HOOK    0
#define configCHECK_FOR_STACK_OVERFLOW  0



/* ============================================================================
 * FONCTIONS API INCLUSES
 * ============================================================================ */

#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelay                  1

#endif /* FREERTOS_CONFIG_H */
