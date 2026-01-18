/* FreeRTOS Configuration for Lab-O-Track (ATMega328P / Arduino Uno) */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

// Code specific definitions
/* RFID Pins */
#define RFID_RX_PIN 2
#define RFID_TX_PIN 3

/* Target RFID tag ID to monitor */
#define TAG_TARGET "OSC-01"

/* Time allowed before alarm triggers */
#define SECURITY_TIMEOUT_MS 6000


/* Task Priorities */
#define TASK_SENSOR_PRIORITY (tskIDLE_PRIORITY + 3)  /* High */
#define TASK_LOGIC_PRIORITY  (tskIDLE_PRIORITY + 2)  /* Middle */
#define TASK_ALARM_PRIORITY  (tskIDLE_PRIORITY + 1)  /* Low */

// FreeRTOS Configuration Parameters

#define configCPU_CLOCK_HZ          ((unsigned long)16000000) /* CPU Frequency (Arduino Uno = 16MHz) */
#define configTICK_RATE_HZ          ((TickType_t)100)         
#define configUSE_PREEMPTION        1                         /* Enable pre-emptive scheduling */
#define configUSE_16_BIT_TICKS      1                         
#define configMAX_PRIORITIES        5
#define configMINIMAL_STACK_SIZE    70                        /* Idle task stack size (in words, not bytes!) */
#define configMAX_TASK_NAME_LEN     8

/* Idle task yields CPU to other tasks of same priority */
#define configIDLE_SHOULD_YIELD     1

/* Software Timers */

#define configUSE_TIMERS                1
#define configTIMER_TASK_PRIORITY       (configMAX_PRIORITIES - 1)
#define configTIMER_TASK_STACK_DEPTH    configMINIMAL_STACK_SIZE
#define configTIMER_QUEUE_LENGTH        3

/* Memory Allocation */
#define configSUPPORT_STATIC_ALLOCATION     0
#define configSUPPORT_DYNAMIC_ALLOCATION    1
#define configTOTAL_HEAP_SIZE               800 /* Total heap size in bytes */

/* Hook Functions */
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0
#define configUSE_MALLOC_FAILED_HOOK    0
#define configCHECK_FOR_STACK_OVERFLOW  0

/* API Function Inclusion */
#define INCLUDE_vTaskSuspend            1
#define INCLUDE_vTaskDelay              1

#endif /* FREERTOS_CONFIG_H */