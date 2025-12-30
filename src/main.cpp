#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <timers.h>
#include "drivers/buzzer/buzzer.h"
#include "drivers/led/led.h"
#include "drivers/rfid/rfid.h"

#define RFID_RX_PIN 2
#define RFID_TX_PIN 3
#define SECURITY_TIMEOUT_MS 6000

#define TASK_SENSOR_PRIORITY (tskIDLE_PRIORITY + 3)
#define TASK_LOGIC_PRIORITY  (tskIDLE_PRIORITY + 2)
#define TASK_ALARM_PRIORITY  (tskIDLE_PRIORITY + 1)


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
  buzzer_pattern_startup();

  xEventQueue = xQueueCreate(5, sizeof(SystemEvent_t));
  xSecurityTimer = xTimerCreate(
      "SecuTimer",
      pdMS_TO_TICKS(SECURITY_TIMEOUT_MS ),
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
  (void)pvParameters;
  bool isTagPresent = false;
  uint16_t ticksSinceLastRead = 0;
  const uint16_t MISSING_THRESHOLD = 15; 
  Serial.println(F("RFID Task Started"));

  for (;;)
  {
    bool readSuccess = false;

    if (rfid.available())
    {
      rfid.read();
      unsigned char *buffer = rfid.get_buffer();

      if (buffer[0] != 0)
      {
        readSuccess = true;
        rfid.clear();
        Serial.println(F("TAG READ!"));
      }
    }

    if (readSuccess)
    {
      ticksSinceLastRead = 0;
      
      if (!isTagPresent)
      {
        isTagPresent = true;
        Serial.println(F(">>> TAG RETURNED"));
        SystemEvent_t evt = EVT_TAG_RETURNED;
        xQueueSend(xEventQueue, &evt, 0);
      }
    }
    else
    {
      ticksSinceLastRead++;
      
      if (ticksSinceLastRead >= MISSING_THRESHOLD && isTagPresent)
      {
        isTagPresent = false;
        Serial.println(F(">>> TAG MISSING"));
        SystemEvent_t evt = EVT_TAG_MISSING;
        xQueueSend(xEventQueue, &evt, 0);
      }
      
      // Évite overflow
      if (ticksSinceLastRead > MISSING_THRESHOLD)
        ticksSinceLastRead = MISSING_THRESHOLD;
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}


static void vTaskLogic(void *pvParameters)
{
  (void)pvParameters;
  SystemEvent_t rxEvent;
  bool timerRunning = false;
  bool alarmActive = false;

  for (;;)
  {
    if (xQueueReceive(xEventQueue, &rxEvent, portMAX_DELAY) == pdPASS)
    {
      switch (rxEvent)
      {
        case EVT_TAG_MISSING:
          if (!timerRunning && !alarmActive)
          {
            xTimerStart(xSecurityTimer, 0);
            timerRunning = true;
            led_off(LED_GREEN);
            led_on(LED_BLUE);
          }
          break;

        case EVT_TAG_RETURNED:
          if (timerRunning)
          {
            xTimerStop(xSecurityTimer, 0);
            timerRunning = false;
            led_off(LED_BLUE);
            led_on(LED_GREEN);
          }
          else if (alarmActive)
          {
            alarmActive = false;
            vTaskSuspend(xAlarmTaskHandle);
            buzzer_off();
            led_all_off();
            led_pattern_success();
            led_on(LED_GREEN);
          }
          break;

        case EVT_TIMER_EXPIRED:
          timerRunning = false;
          alarmActive = true;
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