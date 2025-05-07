#ifndef TIMER_UTILS_H
#define TIMER_UTILS_H

#include <stdint.h>

// === Register Definitions ===

// DPORT (System Clock)
#define DPORT_PERIP_CLK_EN_REG (*((volatile uint32_t *)0x3FF000C0))
#define DPORT_TIMERGROUP0_CLK_EN (1 << 13)

// Timer Group 0 (TIMG0)
#define TIMG0_BASE 0x3FF5F000

// Timer 0 Registers (within TIMG0)
#define TIMG0_T0CONFIG_REG     (*((volatile uint32_t *)(TIMG0_BASE + 0x0000)))
#define TIMG0_T0LO_REG         (*((volatile uint32_t *)(TIMG0_BASE + 0x0004))) // Read Only
#define TIMG0_T0HI_REG         (*((volatile uint32_t *)(TIMG0_BASE + 0x0008))) // Read Only
#define TIMG0_T0UPDATE_REG     (*((volatile uint32_t *)(TIMG0_BASE + 0x000C))) // Write Only
#define TIMG0_T0ALARMLO_REG    (*((volatile uint32_t *)(TIMG0_BASE + 0x0010)))
#define TIMG0_T0ALARMHI_REG    (*((volatile uint32_t *)(TIMG0_BASE + 0x0014)))
#define TIMG0_T0LOADLO_REG     (*((volatile uint32_t *)(TIMG0_BASE + 0x0018)))
#define TIMG0_T0LOADHI_REG     (*((volatile uint32_t *)(TIMG0_BASE + 0x001C)))
#define TIMG0_T0LOAD_REG       (*((volatile uint32_t *)(TIMG0_BASE + 0x0020))) // Write Only

// TIMG0_T0CONFIG_REG Bits
#define TIMG_T0_EN_BIT             (1 << 31) // Timer Enable
#define TIMG_T0_INCREASE_BIT       (1 << 30) // 1: Increment, 0: Decrement
#define TIMG_T0_AUTORELOAD_BIT     (1 << 29) // Auto-reload on alarm
#define TIMG_T0_DIVIDER_POS        13        // Prescaler divider (16 bits)
#define TIMG_T0_DIVIDER_MASK       (0xFFFF << TIMG_T0_DIVIDER_POS)
#define TIMG_T0_ALARM_EN_BIT       (1 << 10) // Enable alarm
// Note: TRM v5.3 seems to have different bit positions than older versions or examples.
// Let's re-verify based on the extracted text or assume common practice.
// Common practice/older TRMs often show EN at bit 31, INCREASE at 30, AUTORELOAD at 29, DIVIDER at 13-28, ALARM_EN at 10.
// Let's use these common positions for now, adjust if needed after testing.
#undef TIMG_T0_EN_BIT
#undef TIMG_T0_INCREASE_BIT
#undef TIMG_T0_AUTORELOAD_BIT
#undef TIMG_T0_DIVIDER_POS
#undef TIMG_T0_DIVIDER_MASK
#undef TIMG_T0_ALARM_EN_BIT

#define TIMG_T0_EN_BIT             (1 << 31) // Timer Enable
#define TIMG_T0_INCREASE_BIT       (1 << 30) // 1: Increment, 0: Decrement
#define TIMG_T0_AUTORELOAD_BIT     (1 << 29) // Auto-reload on alarm
#define TIMG_T0_DIVIDER_POS        13        // Prescaler divider (16 bits)
#define TIMG_T0_DIVIDER_MASK       (0xFFFF << TIMG_T0_DIVIDER_POS)
#define TIMG_T0_ALARM_EN_BIT       (1 << 10) // Enable alarm


// === Constants ===
#define APB_CLK_FREQ 80000000 // 80 MHz
#define TIMER_CLK_FREQ 1000000 // Target 1 MHz for microsecond counting
#define TIMER_PRESCALER (APB_CLK_FREQ / TIMER_CLK_FREQ) // Should be 80

// === Function Prototypes ===
void initHardwareTimer();
uint64_t readHardwareTimer();

// === Function Implementations ===

/**
 * @brief Initializes Timer 0 of Timer Group 0 (TIMG0_T0) for microsecond counting.
 */
void initHardwareTimer() {
    // 1. Enable Timer Group 0 peripheral clock
    DPORT_PERIP_CLK_EN_REG |= DPORT_TIMERGROUP0_CLK_EN;
    // Optional: Reset Timer Group 0 (usually not needed)
    // DPORT_PERIP_RST_EN_REG |= DPORT_TIMERGROUP0_RST_EN;
    // DPORT_PERIP_RST_EN_REG &= ~DPORT_TIMERGROUP0_RST_EN;

    // 2. Configure Timer 0 (TIMG0_T0)
    uint32_t config = 0;

    // Disable timer for configuration
    config &= ~TIMG_T0_EN_BIT;

    // Disable alarm
    config &= ~TIMG_T0_ALARM_EN_BIT;

    // Set counting direction to UP
    config |= TIMG_T0_INCREASE_BIT;

    // Disable auto-reload
    config &= ~TIMG_T0_AUTORELOAD_BIT;

    // Set prescaler
    // TRM: Divisor = 2 to 65536. Value 0 means 65536, Value 1 means 2.
    uint16_t prescaler_val = TIMER_PRESCALER;
    if (prescaler_val < 2) prescaler_val = 2;
    config &= ~TIMG_T0_DIVIDER_MASK; // Clear previous divider value
    config |= (prescaler_val << TIMG_T0_DIVIDER_POS);

    // Apply the configuration (except enable bit)
    TIMG0_T0CONFIG_REG = config;

    // 3. Load initial counter value (optional, start from 0)
    TIMG0_T0LOADLO_REG = 0;
    TIMG0_T0LOADHI_REG = 0;
    TIMG0_T0LOAD_REG = 1; // Trigger the load

    // 4. Enable the timer
    TIMG0_T0CONFIG_REG |= TIMG_T0_EN_BIT;
}

/**
 * @brief Reads the current 64-bit value of the hardware timer (TIMG0_T0).
 *
 * @return The current timer value in microseconds.
 */
uint64_t readHardwareTimer() {
    // 1. Trigger timer value latch
    TIMG0_T0UPDATE_REG = 1;

    // 2. Read latched low and high parts
    uint32_t low = TIMG0_T0LO_REG;
    uint32_t high = TIMG0_T0HI_REG;

    // 3. Combine into 64-bit value
    return ((uint64_t)high << 32) | low;
}

#endif // TIMER_UTILS_H