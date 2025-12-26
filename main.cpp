#include <Arduino.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#include "drivers/buzzer/buzzer.h"
#include "drivers/led/led.h"
#include "drivers/rfid/rfid.h"
#include "FreeRTOSConfig.h"

RFID rfid(RFID_RX_PIN, RFID_TX_PIN);

// Handles FreeRTOS
QueueHandle_t xEventQueue;
TimerHandle_t xSecurityTimer;
TaskHandle_t xAlarmTaskHandle = NULL;

typedef enum
{
  EVT_TAG_MISSING,
  EVT_TAG_RETURNED,
  EVT_TIMER_EXPIRED
} SystemEvent_t;

static void vTaskReadTag(void *pvParameters);
static void vTaskLogic(void *pvParameters);
static void vTaskAlarm(void *pvParameters);

static void vTimerCallback(TimerHandle_t xTimer);

int main(void)
{
  init();
  Serial.begin(9600);
  buzzer_init();
  led_init_all();
  rfid.init();

  led_pattern_startup();

  xEventQueue = xQueueCreate(5, sizeof(SystemEvent_t));
  xSecurityTimer = xTimerCreate(
      "SecuTimer",
      pdMS_TO_TICKS(60000),
      pdFALSE,
      (void *)0,
      vTimerCallback);

  xTaskCreate(vTaskReadTag, "ReadTag", 85, NULL, TASK_SENSOR_PRIORITY, NULL);
  xTaskCreate(vTaskLogic, "Logic", 90, NULL, TASK_LOGIC_PRIORITY, NULL);
  xTaskCreate(vTaskAlarm, "Alarm", 70, NULL, TASK_ALARM_PRIORITY, &xAlarmTaskHandle);

  vTaskSuspend(xAlarmTaskHandle);

  // From here, the kernel takes control. The program never exits here.
  vTaskStartScheduler();

  return 0;
}

static void vTaskReadTag(void *pvParameters)
{
  bool isTagPresent = true;
  uint8_t missingCount = 0;
  const uint8_t THRESHOLD = 5;

  for (;;)
  {
    bool readSuccess = false;

    if (rfid.available())
    {
      rfid.read();
      unsigned char *buffer = rfid.get_buffer();

      // Check if buffer is not empty
      if (buffer[0] != 0)
      {
        readSuccess = true;
        rfid.clear();
      }
    }

    if (readSuccess)
    {
      missingCount = 0;
      if (!isTagPresent)
      {
        isTagPresent = true;
        SystemEvent_t evt = EVT_TAG_RETURNED;
        xQueueSend(xEventQueue, &evt, 0);
      }
    }
    else
    {
      missingCount++;
      if (missingCount >= THRESHOLD && isTagPresent)
      {
        isTagPresent = false;
        SystemEvent_t evt = EVT_TAG_MISSING;
        xQueueSend(xEventQueue, &evt, 0);
      }
    }

    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

static void vTaskLogic(void *pvParameters)
{
  SystemEvent_t rxEvent;
  bool timerRunning = false;

  for (;;)
  {
    if (xQueueReceive(xEventQueue, &rxEvent, portMAX_DELAY) == pdPASS)
    {

      switch (rxEvent){
        case EVT_TAG_MISSING:
          if (!timerRunning)
          {

            xTimerStart(xSecurityTimer, 0);
            timerRunning = true;

            // Visual indicator "Waiting" (Blue)
            led_off(LED_GREEN);
            led_on(LED_BLUE);
          }
          
          break;

        case EVT_TAG_RETURNED:
          if (timerRunning)
          {
            // Case 1 : Returned BEFORE alarm -> Safe
            xTimerStop(xSecurityTimer, 0);
            timerRunning = false;

            // Visual indicator "Safe" (Green)
            led_off(LED_BLUE);
            led_on(LED_GREEN);
          }
          else //TODO: pq un else et pas tout mettre dans le case EVT_TAG_RETURNED sans if ?
          {
            // Case 2 : Returned AFTER alarm -> Resolved

            // Stop the alarm task
            vTaskSuspend(xAlarmTaskHandle);

            // Manual cleanup of outputs (because the alarm task stops abruptly)
            buzzer_off();
            led_all_off();

            // Success pattern
            led_pattern_success();
            led_on(LED_GREEN);
          }
          break;
        case EVT_TIMER_EXPIRED:
          timerRunning = false;

          // Activate the alarm task
          vTaskResume(xAlarmTaskHandle);
          break;
      }
    }
  }
}

static void vTaskAlarm(void *pvParameters)
{
  for (;;)
  {
    led_pattern_alert();
    buzzer_pattern_alert();

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void vTimerCallback(TimerHandle_t xTimer)
{
  SystemEvent_t evt = EVT_TIMER_EXPIRED;
  xQueueSend(xEventQueue, &evt, 0);
}

extern "C" void loop() {}