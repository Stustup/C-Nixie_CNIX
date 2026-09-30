#ifndef DCF77_H_
#define DCF77_H_

#include "stm32g0xx_hal.h"
#include "main.h"

// ------------------------------------------------------------------
// Konfiguration
// ------------------------------------------------------------------

// EN-Pin des DCF77-Moduls (Enable/Power)
#define DCF77_EN_PORT       addon_en_GPIO_Port
#define DCF77_EN_PIN        addon_en_Pin
#define DCF77_EN_ACTIVE     0               // EN High = Modul aus. 

// Signal-Pin (nur zum Einlesen des Pin-Levels im Capture-Callback benoetigt).
// TIM2_CH2 liegt auf dem STM32G051 z.B. auf PA1 -> anpassen, falls anders belegt.
#define DCF77_SIGNAL_PORT   GPIOA
#define DCF77_SIGNAL_PIN    GPIO_PIN_2

// Capture-Kanal (Timer kommt aus CubeMX: extern TIM_HandleTypeDef htim2)
#define DCF77_TIM_CHANNEL  TIM_CHANNEL_3

// ------------------------------------------------------------------
// Zeitstruktur
// ------------------------------------------------------------------
typedef struct {
    uint8_t second;      // 0-59
    uint8_t minute;      // 0-59
    uint8_t hour;        // 0-23 (lokale Zeit, Sommerzeit bereits beruecksichtigt)
    uint8_t is_dst;      // 1 = Sommerzeit (MESZ)
    uint8_t parity_ok;   // 1 = Parity von Minute/Stunde ok
    uint8_t data_valid;  // 1 = mindestens eine gueltige Minute dekodiert
} DCF77_TimeTypeDef;

// ------------------------------------------------------------------
// API
// ------------------------------------------------------------------

// Einmalig aufrufen. htim: Handle des von CubeMX initialisierten Timers
// (z.B. &htim2). Initialisiert EN-Pin (PA2) und interne Zustandsvariablen.
void DCF77_Init(TIM_HandleTypeDef *htim);

// Modul-Power/Enable separat steuern (z.B. fuer Tages-Sync um 0 Uhr)
void DCF77_Enable(void);     // EN-Pin setzen
void DCF77_Disable(void);   // EN-Pin loeschen

// Capture starten/stoppen (inkl. EN-Pin)
// Start: EN auf High + Input-Capture-Interrupt starten (Modul braucht
//        nach Power-On einige Minuten zum Synchronisieren!)
// Stop:  Interrupt stoppen + EN auf Low (Modul aus)
void DCF77_Start(void);
void DCF77_Stop(void);

// Aktuell dekodierte Zeit (Sekunden laufen live mit, gesteuert von den
// Sekundenpulsen des DCF77-Signals)
DCF77_TimeTypeDef DCF77_GetTime(void);

// 1, sobald eine vollstaendige, parity-guetige Minute empfangen wurde
uint8_t DCF77_IsDataValid(void);

// Optionaler Callback nach jeder erfolgreich dekodierten Minute
typedef void (*DCF77_CallbackTypeDef)(DCF77_TimeTypeDef *time);
void DCF77_RegisterCallback(DCF77_CallbackTypeDef callback);

#endif