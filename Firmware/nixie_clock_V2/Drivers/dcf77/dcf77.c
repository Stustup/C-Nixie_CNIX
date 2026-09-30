///  ------------------------ CubeMX-Einstellungen -----------------------
///  (in main.c generiert, NICHT in diesem Treiber!)
///
///  TIM2 -> Channel2:  Input Capture direct mode
///    - Polarity:      On Both Edges   (steigende + fallende Flanke)
///    - IC Selection:  Direct
///    - Prescaler:     Timertakt / 10000 - 1   => 1 Tick = 100 us
///                     (z.B. 64 MHz Timertakt -> Prescaler = 6399)
///    - Counter Mode:  Up
///    - Counter Period (ARR): 65535
///    - NVIC:          TIM2 capture compare interrupt ENABLE
///
///  GPIO:
///    - PA2:  GPIO_Output, Push-Pull, no pull (EN-Pin, wird hier benutzt)
///    - DCF77-Datenpin: TIM2_CH2 (z.B. PA1, AF2) - CubeMX setzt die AF
///  ---------------------------------------------------------------------
///
///  Funktionsweise (DCF77-Protokoll, typischer Empfaenger-Ausgang aktiv low):
///    Der Empfaenger-Ausgang ist im Ruhezustand HIGH. Zu Beginn jeder
///    Sekunde faellt er fuer 100 ms (Bit 0) bzw. 200 ms (Bit 1) auf LOW.
///    In der 59. Sekunde einer Minute bleibt das Signal HIGH (fehlender
///    Puls = Minutenmarke). Eine Minute besteht aus 59 Bits (Bit 0..58).
///
///    Bitlage (DCF77):
///      0        Minutenanfang (immer 0)
///      1-14     zivile Warnbits / Wetter
///      15       Reserveantenne
///      16       Ankündigung Zeitumstellung
///      17/18    Zeitzone (MEZ: 0/1, MESZ: 1/0)
///      19       immer 1
///      20       Beginn Zeitinformation (immer 1)
///      21-27    Minute  (1,2,4,10,20,40,80)
///      28       Parity Minute
///      29-34    Stunde  (1,2,4,10,20,40)
///      35       Parity Stunde
///      36-58    Datum (hier nicht ausgewertet)
///
///    Gesendet wird immer MEZ. Fuer lokale Zeit wird bei MESZ
///    1 Stunde addiert.
///
#include "dcf77.h"

// ------------------------------------------------------------------
// interne Zustandsvariablen
// ------------------------------------------------------------------
static TIM_HandleTypeDef *s_htim = NULL;      // Handle aus CubeMX (htim2)
static DCF77_CallbackTypeDef s_callback = NULL;

static volatile DCF77_TimeTypeDef s_time = {0};

static volatile uint8_t  s_bits[59] = {0};    // Puffer der Minute
static volatile uint8_t  s_bit_index = 0;     // naechste Bitposition (0-58)
static volatile uint8_t  s_in_pulse = 0;      // 1 = Signal aktuell LOW
static volatile uint32_t s_last_event = 0;    // letzter Capture-Zeitpunkt (Ticks)

// Schwellwerte in Ticks (1 Tick = 1ms)
#define TICKS_100MS     100u   // Pulslaenge Bit 0
#define TICKS_200MS     200u   // Pulslaenge Bit 1
#define TICKS_BIT_TH    150u   // Schwelle 0/1
#define TICKS_GAP_TH    1500u  // >1,5 s zwischen Pulsen -> Minutenmarke

// ------------------------------------------------------------------
// Hilfsfunktionen
// ------------------------------------------------------------------

// Even-Parity ueber Bitbereich berechnen
static uint8_t dcf77_parity(const volatile uint8_t *bits, uint8_t start, uint8_t end)
{
    uint8_t p = 0;
    for (uint8_t i = start; i <= end; i++) {
        p ^= bits[i];
    }
    return p;
}

// Dekodiert die empfangene Minute aus s_bits und aktualisiert s_time
static void dcf77_decode_minute(void)
{
    uint8_t bit;
    uint8_t minute = 0, hour = 0;

    // Sicherheitscheck: Bit 20 (Beginn Zeitinformation) muss 1 sein
    if (!s_bits[20]) {
        return;
    }

    // Minute: Bits 21-27 (Gewicht 1,2,4,10,20,40,80)
    bit = 1;
    for (int8_t i = 21; i <= 27; i++) {
        minute += s_bits[i] ? bit : 0;
        if (i == 23) bit = 10; else bit <<= 1;
    }
    if (minute > 59) return;                       // ungueltig

    // Stunde: Bits 29-34 (Gewicht 1,2,4,10,20,40)
    bit = 1;
    for (int8_t i = 29; i <= 34; i++) {
        hour += s_bits[i] ? bit : 0;
        if (i == 31) bit = 10; else bit <<= 1;
    }
    if (hour > 23) return;                         // ungueltig

    // Parity Minute (Bits 21-27) und Stunde (Bits 29-34)
    uint8_t p_min  = dcf77_parity(s_bits, 21, 27);
    uint8_t p_hour = dcf77_parity(s_bits, 29, 34);

    // Zeitzone: Bit 17=1, Bit 18=0 -> MESZ, sonst MEZ
    uint8_t mesz = (s_bits[17] == 1 && s_bits[18] == 0) ? 1 : 0;

    // Lokale Zeit: gesendet wird MEZ, bei Sommerzeit +1h
    if (mesz) {
        hour = (hour + 1) % 24;
    }

    s_time.minute     = minute;
    s_time.hour       = hour;
    s_time.second     = 0;                          // Minutenanfang
    s_time.is_dst     = mesz;
    s_time.parity_ok  = (p_min == s_bits[28]) && (p_hour == s_bits[35]);
    s_time.data_valid = 1;

    if (s_callback != NULL) {
        s_callback((DCF77_TimeTypeDef *)&s_time);
    }
}

// ------------------------------------------------------------------
// HAL Capture-Callback (wird aus dem TIM2-IRQ von HAL aufgerufen)
// ------------------------------------------------------------------
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim != s_htim) return;

    uint32_t now   = HAL_TIM_ReadCapturedValue(htim, DCF77_TIM_CHANNEL);
    uint32_t delta = (now - s_last_event) & 0xFFFFu;  // Tick-Differenz (Overflow-sicher)
    uint8_t  level = HAL_GPIO_ReadPin(DCF77_SIGNAL_PORT, DCF77_SIGNAL_PIN);

    if (level == GPIO_PIN_RESET) {
        // ----- fallende Flanke: Anfang eines Sekundenpulses -----
        if (delta > TICKS_GAP_TH) {
            // >1,5 s ohne Puls -> Minute komplett (Sekunde 59 fehlt)
            if (s_bit_index == 59) {
                dcf77_decode_minute();
            }
            s_bit_index = 0;
            s_time.second = 0;
        } else if (delta > 2 * TICKS_100MS) {
            // ungueltiger Abstand -> resynchronisieren
            s_bit_index = 0;
            s_time.second = 0;
        } else {
            // normaler 1-s-Takt: Sekunde = Bitposition
            s_time.second = (s_bit_index < 59) ? s_bit_index : 0;
        }
        s_in_pulse = 1;
    } else {
        // ----- steigende Flanke: Ende des Sekundenpulses -----
        if (s_in_pulse) {
            if (s_bit_index < 59) {
                s_bits[s_bit_index] = (delta > TICKS_BIT_TH) ? 1 : 0;
                s_bit_index++;
            }
        }
        s_in_pulse = 0;
    }

    s_last_event = now;
}

// ------------------------------------------------------------------
// API
// ------------------------------------------------------------------
void DCF77_Init(TIM_HandleTypeDef *htim)
{
    s_htim = htim;

    // Zustand zuruecksetzen
    s_bit_index  = 0;
    s_in_pulse   = 0;
    s_last_event = 0;
    s_time.data_valid = 0;
}

void DCF77_Enable(void)
{
    HAL_GPIO_WritePin(DCF77_EN_PORT, DCF77_EN_PIN, DCF77_EN_ACTIVE);
}

void DCF77_Disable(void)
{
    HAL_GPIO_WritePin(DCF77_EN_PORT, DCF77_EN_PIN, !DCF77_EN_ACTIVE);
}

void DCF77_Start(void)
{
    s_bit_index  = 0;
    s_in_pulse   = 0;
    s_last_event = HAL_TIM_ReadCapturedValue(s_htim, DCF77_TIM_CHANNEL);
    s_time.data_valid = 0;

    DCF77_Enable();                                     // Modul einschalten
    HAL_TIM_IC_Start_IT(s_htim, DCF77_TIM_CHANNEL);     // Capture starten
}

void DCF77_Stop(void)
{
    HAL_TIM_IC_Stop_IT(s_htim, DCF77_TIM_CHANNEL);
    DCF77_Disable();                                    // Modul ausschalten
}

DCF77_TimeTypeDef DCF77_GetTime(void)
{
    return s_time;
}

uint8_t DCF77_IsDataValid(void)
{
    return s_time.data_valid;
}

void DCF77_RegisterCallback(DCF77_CallbackTypeDef callback)
{
    s_callback = callback;
}