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

/* Tag cible */
#define TAG_TARGET "OSC-01"

/* Timeout de sécurité : 1 minute */
#define SECURITY_TIMEOUT_MS (1UL * 60UL * 1000UL)

/* Priorités des tâches */
#define TASK_SENSOR_PRIORITY (tskIDLE_PRIORITY + 3)  /* High */
#define TASK_LOGIC_PRIORITY  (tskIDLE_PRIORITY + 2)  /* Middle */
#define TASK_ALARM_PRIORITY  (tskIDLE_PRIORITY + 1)  /* Low */

/* ============================================================================
 * CONFIGURATION FREERTOS POUR ATMEGA328P
 * ============================================================================ */

/* Fréquence CPU (Arduino Uno = 16MHz) */
#define configCPU_CLOCK_HZ          ((unsigned long)16000000)

/* Fréquence des ticks (100 Hz = tick toutes les 10ms) */
#define configTICK_RATE_HZ          ((TickType_t)100)

/* Mode préemptif activé */
#define configUSE_PREEMPTION        1

/* Utiliser des ticks 16 bits (économise de la RAM sur AVR) */
#define configUSE_16_BIT_TICKS      1

/* Nombre maximum de priorités */
#define configMAX_PRIORITIES        5

/* Taille minimale de pile (en mots, pas en bytes!) */
#define configMINIMAL_STACK_SIZE    70

/* Longueur max du nom des tâches */
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
 * FONCTIONNALITÉS INCLUSES
 * ============================================================================ */

#define configUSE_MUTEXES                   0
#define configUSE_RECURSIVE_MUTEXES         0
#define configUSE_COUNTING_SEMAPHORES       0
#define configUSE_QUEUE_SETS                0
#define configUSE_TASK_NOTIFICATIONS        1
#define configUSE_TRACE_FACILITY            0
#define configUSE_STATS_FORMATTING_FUNCTIONS 0
#define configUSE_CO_ROUTINES               0
#define configGENERATE_RUN_TIME_STATS       0

/* ============================================================================
 * FONCTIONS API INCLUSES
 * ============================================================================ */

#define INCLUDE_vTaskPrioritySet            0
#define INCLUDE_uxTaskPriorityGet           0
#define INCLUDE_vTaskDelete                 0
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_xResumeFromISR              0
#define INCLUDE_vTaskDelayUntil             0
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetSchedulerState      0
#define INCLUDE_xTaskGetCurrentTaskHandle   0
#define INCLUDE_uxTaskGetStackHighWaterMark 0
#define INCLUDE_xTaskGetIdleTaskHandle      0
#define INCLUDE_eTaskGetState               0
#define INCLUDE_xTimerPendFunctionCall      0
#define INCLUDE_xTaskAbortDelay             0
#define INCLUDE_xTaskGetHandle              0
#define INCLUDE_xTaskResumeFromISR          0

#endif /* FREERTOS_CONFIG_H */
