#include "dcf77.h"

static TIM_HandleTypeDef *s_htim = NULL;      // Handle from CubeMX (htim2)
static DCF77_CallbackTypeDef s_callback = NULL;

static volatile DCF77_TimeTypeDef s_time = {0};

static volatile uint8_t  s_bits[59] = {0};    // buffer of minute
static volatile uint8_t  s_bit_index = 0;     // next bit position
static volatile uint8_t  s_in_pulse = 0;      
static volatile uint32_t s_last_event = 0;    // last capture time

#define DCF77_TICK_US       1000u
#define DCF77_TICKS(us)     (((us) + (DCF77_TICK_US) - 1u) / (DCF77_TICK_US))

// Schwellwerte (in Ticks, automatisch skaliert)
#define PULSE_MIN_TICKS     DCF77_TICKS(60000u)    // shortest valid pulse: 60 ms
#define PULSE_MAX_TICKS     DCF77_TICKS(260000u)   // longest valid pulse: 260 ms
#define PULSE_BIT_TH_TICKS  DCF77_TICKS(150000u)   // threshold: 150 ms

#define GAP_MIN_TICKS       DCF77_TICKS(600000u)   // normal wait time from 600 ms
#define GAP_MAX_TICKS       DCF77_TICKS(1100000u)  // to 1100 ms
#define GAP_MINUTE_TICKS    DCF77_TICKS(1500000u)   // >1,5 s -> new minute mark

// Calculate parity
static uint8_t dcf77_parity(const volatile uint8_t *bits, uint8_t start, uint8_t end)
{
    uint8_t p = 0;
    for (uint8_t i = start; i <= end; i++) {
        p ^= bits[i];
    }
    return p;
}

// Decodes minute bits and calls callback if finished
static void dcf77_decode_minute(void) {

    uint8_t bit;
    uint8_t minute = 0, hour = 0, day = 0, weekday = 0, month = 0, year = 0;

    // Security check -> Bit 20 has to be 1 if valid
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

    // Hour: Bits 29-34 (Gewicht 1,2,4,8,10,20)
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

    // Parity minute (Bits 21-27) and hour (Bits 29-34) and date (Bits 36-57)
    uint8_t p_min  = dcf77_parity(s_bits, 21, 27);
    uint8_t p_hour = dcf77_parity(s_bits, 29, 34);
    uint8_t p_date = dcf77_parity(s_bits, 36, 57);

    // Timezone: Bit 17=1, Bit 18=0 -> MESZ, otherwise MEZ
    uint8_t mesz = (s_bits[17] == 1 && s_bits[18] == 0) ? 1 : 0;

    // Local time: MEZ gets sent, summertime adds +1h
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
    s_time.second     = 0;                          
    s_time.is_dst     = mesz;
    s_time.parity_ok  = (p_min == s_bits[28]) && (p_hour == s_bits[35]) && (p_date == s_bits[58]);
    s_time.data_valid = 1;

    if (s_callback != NULL) {
        s_callback((DCF77_TimeTypeDef *)&s_time);
    }
}

/**
 * @brief Gets called from the TIM2-IRQ
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {

    if (htim != s_htim) return;

    uint32_t now   = HAL_TIM_ReadCapturedValue(htim, DCF77_TIM_CHANNEL);
    uint32_t delta = (now - s_last_event) & 0xFFFFu;  // Tick-difference (Overflow-safe)
    uint8_t  level = HAL_GPIO_ReadPin(DCF77_SIGNAL_PORT, DCF77_SIGNAL_PIN);

    if (level == GPIO_PIN_SET) {
        // Rising Edge -> Start of new pulse
        // delta = LOW-time since last pulse (normal 800/900 ms)
        if (delta >= GAP_MINUTE_TICKS) {
            // >1,5 s without pulse -> new minute mark
            if (s_bit_index == 59) {
                dcf77_decode_minute();
            }
            s_bit_index = 0;
            s_time.second = 0;
        } else if (delta >= GAP_MIN_TICKS && delta <= GAP_MAX_TICKS) {
            s_time.second = (s_bit_index < 59) ? s_bit_index : 0;
        } else {
            // invalid pulse 
            s_bit_index = 0;
            s_time.second = 0;
        }
        s_in_pulse = 1;
    } else {
        // Rising edge: end of pulse -> safe bit
        if (s_in_pulse) {
            if (delta >= PULSE_MIN_TICKS && delta <= PULSE_MAX_TICKS) {
                // valid pulse: 100/200 ms -> safe bit
                if (s_bit_index < 59) {
                    s_bits[s_bit_index] = (delta > PULSE_BIT_TH_TICKS) ? 1 : 0;
                    s_bit_index++;
                }
            } else {
                // invalid pulse
                s_bit_index = 0;
                s_time.second = 0;
            }
        }
        s_in_pulse = 0;
    }

    s_last_event = now;
}

void DCF77_Init(TIM_HandleTypeDef *htim)
{
    s_htim = htim;

    // reset state
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

    DCF77_Enable();                                                     // turn on module
    HAL_TIM_IC_Start_IT(s_htim, DCF77_TIM_CHANNEL);       // start capture
}

void DCF77_Stop(void)
{
    HAL_TIM_IC_Stop_IT(s_htim, DCF77_TIM_CHANNEL);
    DCF77_Disable();                                                     // turn off module
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