#include "drivers/buzzer/buzzer.h"
#include "drivers/rfid/rfid.h"
#include "drivers/i2c/i2c.h"
#include "drivers/led/led.h"
#include <avr/io.h>
#include <util/delay.h>
#include <Arduino.h>
#include <string.h>

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

#define PRIORITY_RFID_SCAN      3
#define PRIORITY_TIMER_MANAGER  2
#define PRIORITY_ALERT_HANDLER  2
#define PRIORITY_COMM_RPI       1

typedef enum {
    STATE_PRESENT,      // Équipement présent sur son tag
    STATE_ABSENT,       // Équipement absent, timer en cours
    STATE_ALERT         // Équipement absent trop longtemps, alerte active
} equipment_state_t;

typedef struct {
    uint8_t tag_id[RFID_DATA_SIZE];  // ID du tag RFID
    char name[16];                    // Nom de l'équipement (ex: "OSC-01")
    uint32_t timer_duration_sec;      // Durée avant alerte (secondes)
    uint32_t absent_since;            // Timestamp du départ (en ticks)
    equipment_state_t state;          // État actuel
    bool is_registered;               // Tag enregistré ?
} equipment_t;

typedef struct {
    char equipment_name[16];
    equipment_state_t state;
    uint32_t elapsed_time_sec;
} alert_message_t;

// ============================================================================
// VARIABLES GLOBALES ET RESSOURCES FREERTOS
// ============================================================================

// Table des équipements (max 5 pour l'exemple)
equipment_t g_equipment;

// Handles des tâches
TaskHandle_t xRfidScanTaskHandle = NULL;
TaskHandle_t xTimerCheckTaskHandle = NULL;
TaskHandle_t xAlertTaskHandle = NULL;
TaskHandle_t xCommunicationTaskHandle = NULL;

// Sémaphores et mutex
SemaphoreHandle_t xEquipmentMutex;        // Protection accès à g_equipments
SemaphoreHandle_t xAlertSemaphore;        // Signale qu'une alerte doit être déclenchée

// Queues
QueueHandle_t xAlertQueue;                // Queue des alertes vers Raspberry Pi
QueueHandle_t xRfidEventQueue;            // Queue des événements RFID

// Variables partagées
volatile bool g_system_armed = true;      // Système activé/désactivé

// ============================================================================
// FONCTIONS UTILITAIRES
// ============================================================================

/**
 * Enregistre un nouvel équipement dans le système
 */
bool register_equipment(const uint8_t *tag_id, const char *name, uint32_t timer_sec) {
    
    xSemaphoreTake(xEquipmentMutex, portMAX_DELAY);
    
    memcpy(g_equipment.tag_id, tag_id, RFID_DATA_SIZE);
    strncpy(g_equipment.name, name, 15);
    g_equipment.name[15] = '\0';
    g_equipment.timer_duration_sec = timer_sec;
    g_equipment.state = STATE_PRESENT;
    g_equipment.is_registered = true;
    
    xSemaphoreGive(xEquipmentMutex);
    return true;
}

// ============================================================================
// TÂCHE 1 : SCAN RFID (Priorité HAUTE)
// ============================================================================
// Responsabilité : Détection en temps réel de la présence/absence des tags
// Critique car doit réagir immédiatement aux changements physiques
// ============================================================================

void vRfidScanTask(void *pvParameters) {
    (void) pvParameters;
    
    bool tag_present = false;
    int counter = 0;
    
    // Initialisation
    rfid_init();
    led_init_all();
    led_pattern_startup();
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(5000); // Scan toutes les 5000ms
    
    for (;;) {
        counter++;
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        
        if (!g_system_armed) continue;
        
        // Lecture du tag RFID
        //if (rfid_read_tag_id(g_equipment.tag_id)) {
        if (counter % 3 == 0) { // Simuler détection alternée pour test
            led_toggle(LED_BLUE); // Feedback visuel de lecture
            
            xSemaphoreTake(xEquipmentMutex, portMAX_DELAY);
                
            // Tag détecté = équipement PRÉSENT
            if (g_equipment.state == STATE_ABSENT || g_equipment.state == STATE_ALERT) {
                // Retour de l'équipement
                g_equipment.state = STATE_PRESENT;
                g_equipment.absent_since = 0;
                
                // Feedback positif
                led_on(LED_GREEN);
                buzzer_pattern_success();
                vTaskDelay(pdMS_TO_TICKS(100));
                led_off(LED_GREEN);
            }
                
            tag_present = true;
            
            xSemaphoreGive(xEquipmentMutex);
        }
        
        // Vérifier les absences (tags qui étaient là mais plus détectés)
        xSemaphoreTake(xEquipmentMutex, portMAX_DELAY);
        
        if (!g_equipment.is_registered) continue;
        
        // Si le tag n'est plus détecté et était présent
        if (!tag_present && g_equipment.state == STATE_PRESENT) {
            // Marquer comme absent et démarrer le timer
            g_equipment.state = STATE_ABSENT;
            g_equipment.absent_since = xTaskGetTickCount();
            
            // Feedback visuel
            led_on(LED_BLUE);
            buzzer_beep(100);
            vTaskDelay(pdMS_TO_TICKS(100));
            led_off(LED_BLUE);
        }
        
        // Reset pour la prochaine itération
        tag_present = false;
    
        xSemaphoreGive(xEquipmentMutex);
        
        rfid_clear(); // Nettoyer le buffer UART
    }
}

// ============================================================================
// TÂCHE 2 : VÉRIFICATION TIMER (Priorité MOYENNE - 2)
// ============================================================================
// Responsabilité : Surveiller les délais et déclencher les alertes
// Moins critique que RFID mais doit être régulière
// ============================================================================

void vTimerCheckTask(void *pvParameters) {
    (void) pvParameters;
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000); // Vérif toutes les 1s
    
    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        
        if (!g_system_armed) continue;
        
        xSemaphoreTake(xEquipmentMutex, portMAX_DELAY);
        
        TickType_t current_time = xTaskGetTickCount();
            
        if (!g_equipment.is_registered) continue;
        
        // Si équipement absent, vérifier le délai
        if (g_equipment.state == STATE_ABSENT) {
            TickType_t elapsed_ticks = current_time - g_equipment.absent_since;
            uint32_t elapsed_sec = elapsed_ticks / configTICK_RATE_HZ;
            
            // Délai dépassé ?
            if (elapsed_sec >= g_equipment.timer_duration_sec) {
                g_equipment.state = STATE_ALERT;
                
                // Préparer le message d'alerte
                alert_message_t alert_msg;
                strncpy(alert_msg.equipment_name, g_equipment.name, 15);
                alert_msg.state = STATE_ALERT;
                alert_msg.elapsed_time_sec = elapsed_sec;
                
                // Envoyer à la queue d'alertes (non-bloquant)
                xQueueSend(xAlertQueue, &alert_msg, 0);
                
                // Signaler la tâche d'alerte
                xSemaphoreGive(xAlertSemaphore);
            }
        }
        
        xSemaphoreGive(xEquipmentMutex);
    }
}

// ============================================================================
// TÂCHE 3 : GESTION ALERTES (Priorité HAUTE - 3)
// ============================================================================
// Responsabilité : Déclencher alarmes sonores/visuelles
// Haute priorité car alerte de sécurité
// ============================================================================

void vAlertTask(void *pvParameters) {
    (void) pvParameters;
    
    buzzer_init();
    alert_message_t alert_msg;
    
    for (;;) {
        // Attendre qu'une alerte soit signalée
        if (xSemaphoreTake(xAlertSemaphore, portMAX_DELAY) == pdTRUE) {
            
            // Récupérer les détails de l'alerte depuis la queue
            if (xQueueReceive(xAlertQueue, &alert_msg, 0) == pdTRUE) {
                
                // ALERTE ACTIVE : Pattern sonore et visuel
                for (uint8_t cycle = 0; cycle < 20; cycle++) {
                    // LED Rouge clignotante
                    led_on(LED_RED);
                    buzzer_on();
                    vTaskDelay(pdMS_TO_TICKS(200));
                    
                    led_off(LED_RED);
                    buzzer_off();
                    vTaskDelay(pdMS_TO_TICKS(200));
                }
                
                // Pattern sonore d'alerte prolongé
                buzzer_pattern_alert();
                
                // Garder LED rouge allumée tant que l'alerte est active
                led_on(LED_RED);
                
                // Vérifier périodiquement si l'équipement est revenu
                while (1) {
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    
                    xSemaphoreTake(xEquipmentMutex, portMAX_DELAY);
                    
                    bool still_alert = false;
                    if (strcmp(g_equipment.name, alert_msg.equipment_name) == 0) {
                        if (g_equipment.state == STATE_ALERT) {
                            still_alert = true;
                            // Rappel sonore
                            buzzer_pattern_warning();
                        }
                        break;
                    }
                    
                    xSemaphoreGive(xEquipmentMutex);
                    
                    if (!still_alert) {
                        // Équipement revenu, arrêter l'alerte
                        led_off(LED_RED);
                        led_on(LED_GREEN);
                        buzzer_pattern_success();
                        vTaskDelay(pdMS_TO_TICKS(500));
                        led_off(LED_GREEN);
                        break;
                    }
                }
            }
        }
    }
}

// ============================================================================
// TÂCHE 4 : COMMUNICATION RASPBERRY PI (Priorité BASSE - 1)
// ============================================================================
// Responsabilité : Envoyer état système vers Raspberry Pi via Serial
// Priorité basse car non critique pour le fonctionnement temps réel
// ============================================================================

void vCommunicationTask(void *pvParameters) {
    (void) pvParameters;
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(5000); // Envoi toutes les 5s
    
    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        
        // Construire et envoyer le rapport d'état
        xSemaphoreTake(xEquipmentMutex, portMAX_DELAY);
        
        Serial.println("=== STATUS REPORT ===");
        
        Serial.print("Equipment: ");
        Serial.print(g_equipment.name);
        Serial.print(" | State: ");
        
        switch (g_equipment.state) {
            case STATE_PRESENT:
                Serial.print("PRESENT");
                break;
            case STATE_ABSENT:
                Serial.print("ABSENT (");
                TickType_t elapsed = (xTaskGetTickCount() - g_equipment.absent_since) / configTICK_RATE_HZ;
                Serial.print(elapsed);
                Serial.print("s/");
                Serial.print(g_equipment.timer_duration_sec);
                Serial.print("s)");
                break;
            case STATE_ALERT:
                Serial.print("ALERT!");
                break;
        }
        Serial.println();
        
        Serial.println("====================");
        
        xSemaphoreGive(xEquipmentMutex);
    }
}

// ============================================================================
// CONFIGURATION ET DÉMARRAGE
// ============================================================================

void setup(void) {
    Serial.begin(9600);
    delay(1000);  // Attendre l'initialisation du port Serial
    
    // Création des sémaphores et mutex
    xEquipmentMutex = xSemaphoreCreateMutex();
    xAlertSemaphore = xSemaphoreCreateBinary();
    
    // Création des queues
    xAlertQueue = xQueueCreate(5, sizeof(alert_message_t));
    xRfidEventQueue = xQueueCreate(10, sizeof(uint8_t) * RFID_DATA_SIZE);
    
    // Enregistrement d'équipements de test
    uint8_t tag_osc[RFID_DATA_SIZE] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30};
    register_equipment(tag_osc, "OSC-01", 900);  // 15 min
    
    // Création des tâches avec priorités justifiées
    xTaskCreate(
        vRfidScanTask,
        "RFID_Scan",
        256,
        NULL,
        PRIORITY_RFID_SCAN,
        &xRfidScanTaskHandle
    );
    
    xTaskCreate(
        vTimerCheckTask,
        "Timer_Check",
        128,
        NULL,
        PRIORITY_TIMER_MANAGER,
        &xTimerCheckTaskHandle
    );
    
    xTaskCreate(
        vAlertTask,
        "Alert",
        128,
        NULL,
        PRIORITY_ALERT_HANDLER,
        &xAlertTaskHandle
    );
    
    xTaskCreate(
        vCommunicationTask,
        "Comm_RPI",
        256,
        NULL,
        PRIORITY_COMM_RPI,
        &xCommunicationTaskHandle
    );
}

void loop() {
    // loop() reste vide ; FreeRTOS gère le multitâche
    // Les tâches s'exécutent via le scheduler FreeRTOS
}