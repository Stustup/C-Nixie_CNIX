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

#define DCF77_TICK_US       1000u
#define DCF77_TICKS(us)     (((us) + (DCF77_TICK_US) - 1u) / (DCF77_TICK_US))

// Schwellwerte (in Ticks, automatisch skaliert)
#define PULSE_MIN_TICKS     DCF77_TICKS(60000u)    // kuerzester gueltiger Puls: 60 ms
#define PULSE_MAX_TICKS     DCF77_TICKS(260000u)   // laengster gueltiger Puls: 260 ms
#define PULSE_BIT_TH_TICKS  DCF77_TICKS(150000u)   // Schwelle Bit 0 / Bit 1: 150 ms

// Abstand zwischen Pulsende und naechstem Pulsbeginn (HIGH-Phase):
//   normal 800/900 ms, in Sekunde 59 (Minutenmarke) ~1,9 s
#define GAP_MIN_TICKS       DCF77_TICKS(600000u)   // normaler Abstand ab 600 ms
#define GAP_MAX_TICKS       DCF77_TICKS(1100000u)  // normaler Abstand bis 1100 ms
#define GAP_MINUTE_TICKS    DCF77_TICKS(1500000u)   // >1,5 s -> Minutenmarke

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
static void dcf77_decode_minute(void) {

    uint8_t bit;
    uint8_t minute = 0, hour = 0, day = 0, weekday = 0, month = 0, year = 0;

    // Sicherheitscheck: Bit 20 (Beginn Zeitinformation) muss 1 sein
    if (!s_bits[20]) {
        return;
    }

    // Minute: Bits 21-27 (Gewicht 1,2,4,8,10,20,40)
    bit = 1;
    for (int8_t i = 21; i <= 27; i++) {
        minute += s_bits[i] ? bit : 0;
        if (i == 24) bit = 10; else bit <<= 1;
    }
    if (minute > 59) return;                        // invalid

    // Stunde: Bits 29-34 (Gewicht 1,2,4,8,10,20)
    bit = 1;
    for (int8_t i = 29; i <= 34; i++) {
        hour += s_bits[i] ? bit : 0;
        if (i == 32) bit = 10; else bit <<= 1;
    }
    if (hour > 23) return;                          // invalid

    // Calendarday: Bits 36-41 (1,2,4,8,10,20)
    bit = 1;
    for (int8_t i = 36; i <= 41; i++) {
        day += s_bits[i] ? bit : 0;
        if (i == 39) bit = 10; else bit <<= 1;
    }
    if (day > 31) return;                           // invalid

    //Weekday: Bits 42-44 (1,2,4)
    bit = 1;
    for (int8_t i = 42; i <= 44; i++) {
        weekday += s_bits[i] ? bit : 0;
        bit <<= 1;
    }
    if (weekday > 7 || weekday == 0) return;        // invalid

    //Calendarmonth: Bits 45-49 (1,2,4,8,10)
    bit = 1;
    for (int8_t i = 45; i <= 49; i++) {
        month += s_bits[i] ? bit : 0;
        if (i == 48) bit = 10; else bit <<= 1;
    }
    if (month > 12) return;                         // invalid

    //Calendaryear: Bits 50-57 (1,2,4,8,10,20,40,80)
    bit = 1;
    for (int8_t i = 50; i <= 57; i++) {
        year += s_bits[i] ? bit : 0;
        if (i == 53) bit = 10; else bit <<= 1;
    }
    if (year > 99) return;                       // ungueltig

    // Parity Minute (Bits 21-27) und Stunde (Bits 29-34)
    uint8_t p_min  = dcf77_parity(s_bits, 21, 27);
    uint8_t p_hour = dcf77_parity(s_bits, 29, 34);
    uint8_t p_date = dcf77_parity(s_bits, 36, 57);

    // Zeitzone: Bit 17=1, Bit 18=0 -> MESZ, sonst MEZ
    uint8_t mesz = (s_bits[17] == 1 && s_bits[18] == 0) ? 1 : 0;

    // Lokale Zeit: gesendet wird MEZ, bei Sommerzeit +1h
    /*
    if (mesz) {
        hour = (hour + 1) % 24;
    }
    */

    s_time.day        = day;
    s_time.weekday    = weekday;
    s_time.month      = month;
    s_time.year       = year;
    s_time.minute     = minute;
    s_time.hour       = hour;
    s_time.second     = 0;                          // Minutenanfang
    s_time.is_dst     = mesz;
    s_time.parity_ok  = (p_min == s_bits[28]) && (p_hour == s_bits[35]) && (p_date == s_bits[58]);
    s_time.data_valid = 1;

    if (s_callback != NULL) {
        s_callback((DCF77_TimeTypeDef *)&s_time);
    }
}

// ------------------------------------------------------------------
// HAL Capture-Callback (wird aus dem TIM2-IRQ von HAL aufgerufen)
// ------------------------------------------------------------------

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {

    if (htim != s_htim) return;

    uint32_t now   = HAL_TIM_ReadCapturedValue(htim, DCF77_TIM_CHANNEL);
    uint32_t delta = (now - s_last_event) & 0xFFFFu;  // Tick-Differenz (Overflow-sicher)
    uint8_t  level = HAL_GPIO_ReadPin(DCF77_SIGNAL_PORT, DCF77_SIGNAL_PIN);

    if (level == GPIO_PIN_SET) {
        // ----- steigende Flanke: Anfang eines neuen Sekundenpulses -----
        // delta = LOW-Abstand seit dem letzten Pulsende (normal 800/900 ms)
        if (delta >= GAP_MINUTE_TICKS) {
            // >1,5 s ohne Puls -> Minutenmarke (Sekunde 59 hatte keinen Puls)
            if (s_bit_index == 59) {
                dcf77_decode_minute();
            }
            s_bit_index = 0;
            s_time.second = 0;
        } else if (delta >= GAP_MIN_TICKS && delta <= GAP_MAX_TICKS) {
            // normaler 1-s-Takt: Sekunde = Bitposition
            s_time.second = (s_bit_index < 59) ? s_bit_index : 0;
        } else {
            // ungueltiger Abstand (Stoerung) -> resynchronisieren
            s_bit_index = 0;
            s_time.second = 0;
        }
        s_in_pulse = 1;
    } else {
        // ----- steigende Flanke: Ende des Sekundenpulses -> Bit speichern -----
        if (s_in_pulse) {
            if (delta >= PULSE_MIN_TICKS && delta <= PULSE_MAX_TICKS) {
                // gueltiger Puls: 100/200 ms -> Bit uebernehmen
                if (s_bit_index < 59) {
                    s_bits[s_bit_index] = (delta > PULSE_BIT_TH_TICKS) ? 1 : 0;
                    s_bit_index++;
                }
            } else {
                // Stoerpuls -> verwerfen und resynchronisieren
                s_bit_index = 0;
                s_time.second = 0;
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