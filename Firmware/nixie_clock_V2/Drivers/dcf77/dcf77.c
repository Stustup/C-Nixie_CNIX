#include "dcf77.h"

// --- Static Variables ---
extern TIM_HandleTypeDef htim2;
static DCF77_TimeTypeDef dcf77_time = {0};
static DCF77_CallbackTypeDef dcf77_callback = NULL;

// DCF77 bit buffer (59 bits per minute)
static uint8_t dcf77_bits[59] = {0};
static uint8_t bit_index = 0;
static uint8_t second_marker_detected = 0;

// Timer capture values
static uint32_t previous_capture = 0;
static uint32_t current_capture = 0;
static uint8_t is_rising_edge = 1; // Start with rising edge

// --- Helper Functions ---
// Convert BCD to decimal
static uint8_t bcd_to_decimal(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

// Calculate parity for a range of bits
static uint8_t calculate_parity(uint8_t *bits, uint8_t start, uint8_t end) {
    uint8_t parity = 0;
    for (uint8_t i = start; i <= end; i++) {
        parity ^= bits[i];
    }
    return parity;
}

// --- Timer Capture Callback ---
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim != &htim2) return;

    current_capture = HAL_TIM_ReadCapturedValue(htim, DCF77_TIMER_CHANNEL);
    uint32_t pulse_width_us = 0;

    // Calculate pulse width in microseconds
    if (current_capture > previous_capture) {
        pulse_width_us = (current_capture - previous_capture) * 1; // Assuming 1us timer resolution
    } else {
        // Overflow case
        pulse_width_us = (0xFFFFFFFF - previous_capture + current_capture) * 1;
    }

    // Detect second marker (500ms low or high)
    if (pulse_width_us >= (DCF77_SECOND_MARK_US - DCF77_TOLERANCE_US) &&
        pulse_width_us <= (DCF77_SECOND_MARK_US + DCF77_TOLERANCE_US)) {
        second_marker_detected = 1;
        bit_index = 0; // Reset bit index for new minute
    }
    // Detect bit 0 (100ms low, 200ms high)
    else if (pulse_width_us >= (DCF77_BIT_0_LOW_US - DCF77_TOLERANCE_US) &&
             pulse_width_us <= (DCF77_BIT_0_LOW_US + DCF77_TOLERANCE_US)) {
        if (is_rising_edge) {
            // This was a low pulse, next should be high
            is_rising_edge = 0;
        } else {
            // This was a high pulse, store bit 0
            if (bit_index < 59) {
                dcf77_bits[bit_index++] = 0;
            }
            is_rising_edge = 1;
        }
    }
    // Detect bit 1 (200ms low, 100ms high)
    else if (pulse_width_us >= (DCF77_BIT_1_LOW_US - DCF77_TOLERANCE_US) &&
             pulse_width_us <= (DCF77_BIT_1_LOW_US + DCF77_TOLERANCE_US)) {
        if (is_rising_edge) {
            // This was a low pulse, next should be high
            is_rising_edge = 0;
        } else {
            // This was a high pulse, store bit 1
            if (bit_index < 59) {
                dcf77_bits[bit_index++] = 1;
            }
            is_rising_edge = 1;
        }
    }

    previous_capture = current_capture;

    // Check if we have received a complete minute (59 bits)
    if (bit_index >= 59 && second_marker_detected) {
        // Decode time
        uint8_t minute_bcd = (dcf77_bits[1] << 6) | (dcf77_bits[2] << 5) | (dcf77_bits[3] << 4) | (dcf77_bits[4] << 3) | 
                            (dcf77_bits[5] << 2) | (dcf77_bits[6] << 1) | dcf77_bits[7];
        uint8_t hour_bcd = (dcf77_bits[20] << 5) | (dcf77_bits[21] << 4) | (dcf77_bits[22] << 3) | 
                           (dcf77_bits[23] << 2) | (dcf77_bits[24] << 1) | dcf77_bits[25];

        dcf77_time.minute = bcd_to_decimal(minute_bcd);
        dcf77_time.hour = bcd_to_decimal(hour_bcd);
        dcf77_time.second = 0; // Second is always 0 at the start of a new minute

        // Validate parity (optional but recommended)
        uint8_t minute_parity = calculate_parity(dcf77_bits, 1, 28); // Parity for bits 1-28
        uint8_t hour_parity = calculate_parity(dcf77_bits, 29, 58); // Parity for bits 29-58
        dcf77_time.parity_ok = (minute_parity == dcf77_bits[28]) && (hour_parity == dcf77_bits[58]);
        dcf77_time.data_valid = 1;

        // Reset for next minute
        bit_index = 0;
        second_marker_detected = 0;

        // Call callback if registered
        if (dcf77_callback != NULL) {
            dcf77_callback(&dcf77_time);
        }
    }
}

// --- Public Functions ---
void DCF77_Init(void) {
    // Start Timer
    HAL_TIM_IC_Start_IT(&htim2, DCF77_TIMER_CHANNEL);

    // Initialize variables
    previous_capture = 0;
    current_capture = 0;
    bit_index = 0;
    second_marker_detected = 0;
    dcf77_time.data_valid = 0;
}

void DCF77_Start(void) {
    HAL_TIM_IC_Start_IT(&htim2, DCF77_TIMER_CHANNEL);
}

void DCF77_Stop(void) {
    HAL_TIM_IC_Stop_IT(&htim2, DCF77_TIMER_CHANNEL);
}

DCF77_TimeTypeDef DCF77_GetTime(void) {
    return dcf77_time;
}

uint8_t DCF77_IsDataValid(void) {
    return dcf77_time.data_valid;
}

void DCF77_RegisterCallback(DCF77_CallbackTypeDef callback) {
    dcf77_callback = callback;
}

// --- IRQ Handler ---
void DCF77_TIMER_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim2);
}