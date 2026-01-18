#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "drivers/buzzer/buzzer.h"
#include "drivers/led/led.h"
#include "drivers/rfid/rfid.h"
#include "drivers/i2c/i2c_slave.h"


RFID rfid(RFID_RX_PIN, RFID_TX_PIN); // Instantiate RFID object

// Handles FreeRTOS
QueueHandle_t xEventQueue;
TimerHandle_t xSecurityTimer;
TaskHandle_t xAlarmTaskHandle = NULL;

typedef enum
{
  EVT_TAG_MISSING,
  EVT_TAG_RETURNED,
  EVT_TIMER_EXPIRED,
  EVT_I2C_COMMAND
} SystemEvent_t;

static void vTaskReadTag(void *pvParameters);
static void vTaskLogic(void *pvParameters);
static void vTaskAlarm(void *pvParameters);
static void vTimerCallback(TimerHandle_t xTimer);

int main(void)
{

  // Intialize hardware
  buzzer_init();
  led_init_all();
  rfid.init();
  i2c_slave_init();
  
  // Initial LED and Buzzer patterns
  led_pattern_startup();
  buzzer_pattern_startup();

  // Create FreeRTOS objects
  xEventQueue = xQueueCreate(3, sizeof(SystemEvent_t));
  xSecurityTimer = xTimerCreate(NULL,pdMS_TO_TICKS(SECURITY_TIMEOUT_MS),pdFALSE,(void *)0,vTimerCallback);

  // Create FreeRTOS tasks
  xTaskCreate(vTaskReadTag, "ReadTag", 85, NULL, TASK_SENSOR_PRIORITY, NULL);
  xTaskCreate(vTaskLogic, "Logic", 90, NULL, TASK_LOGIC_PRIORITY, NULL);
  xTaskCreate(vTaskAlarm, "Alarm", 70, NULL, TASK_ALARM_PRIORITY, &xAlarmTaskHandle);

  // Start with alarm task suspended
  vTaskSuspend(xAlarmTaskHandle);

  // Start the scheduler
  vTaskStartScheduler();

  return 0;
}


static void vTaskReadTag(void *)
{
  bool isTagPresent = true;
  uint8_t missingCount = 0;
  const uint8_t THRESHOLD = 5;


  for (;;)
  {
    bool readSuccess = false;
    // If the tag is detected, we read it
    if (rfid.available())
    {
      rfid.read();
      unsigned char *buffer = rfid.get_buffer();

      if (buffer[0] != 0)
      {
        readSuccess = true;
        rfid.clear();
      }
    }

    if (readSuccess)
    {
      missingCount = 0;
      // Send event only if the tag was previously missing
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
      // Send event only if the tag was previously present and threshold is reached
      if (missingCount >= THRESHOLD && isTagPresent)
      {
        isTagPresent = false;
        if (missingCount > THRESHOLD)missingCount = THRESHOLD; // Prevent overflow
        SystemEvent_t evt = EVT_TAG_MISSING;
        xQueueSend(xEventQueue, &evt, 0);
      }
    }

    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}


static void vTaskLogic(void *)
{
  SystemEvent_t rxEvent;
  bool timerRunning = false;
  bool alarmActive = false;
  i2c_slave_set_status(STATUS_TAG_PRESENT);

  for (;;)
  {
    // Check I2C commands from the RPi (non-blocking)
    uint8_t i2c_cmd = i2c_slave_get_pending_command();
    if (i2c_cmd == CMD_STOP_ALARM && alarmActive) {
      // The RPi acknowledged the alarm
      vTaskSuspend(xAlarmTaskHandle);
      buzzer_off();
      led_all_off();
      alarmActive = false;
    }

    if (xQueueReceive(xEventQueue, &rxEvent, pdMS_TO_TICKS(100)) == pdPASS)
    {
      switch (rxEvent)
      {
        case EVT_TAG_MISSING:
          // Start the security timer only if not already running and alarm not active
          if (!timerRunning && !alarmActive)
          {
            xTimerStart(xSecurityTimer, 0);
            timerRunning = true;
            led_off(LED_GREEN);
            led_on(LED_BLUE);
            i2c_slave_set_status(STATUS_TIMER_RUNNING);
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
            // I2C status update
            i2c_slave_set_status(STATUS_TAG_PRESENT);
          }
          else if (alarmActive)
          {
            // Case 2 : Returned AFTER alarm -> Resolved
            vTaskSuspend(xAlarmTaskHandle);
            buzzer_off();
            led_all_off();
            alarmActive = false;
            led_pattern_success();
            led_on(LED_GREEN);
            // I2C status update
            i2c_slave_set_status(STATUS_TAG_PRESENT);
          }
          break;
        case EVT_TIMER_EXPIRED:
          timerRunning = false;
          alarmActive = true;
          vTaskResume(xAlarmTaskHandle);
          // I2C status update
          i2c_slave_set_status(STATUS_ALARM_ACTIVE);
          break;
        case EVT_I2C_COMMAND:
          // Handled outside the switch via i2c_slave_get_pending_command()
          break;
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

static void vTaskAlarm(void *)
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
  // Timer expired -> send event to logic task so he can activate the alarm
  SystemEvent_t evt = EVT_TIMER_EXPIRED;
  xQueueSend(xEventQueue, &evt, 0);
}
