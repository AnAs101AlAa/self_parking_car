// === Register Definitions ===
// DPORT (System)
#define DPORT_PERIP_CLK_EN_REG (*((volatile uint32_t *)0x3FF000C0))
#define DPORT_LEDC_CLK_EN (1 << 18)

// === GPIO Register Definitions ===
#define GPIO_ENABLE_REG      (*((volatile uint32_t *)0x3FF44020))
#define GPIO_ENABLE1_REG     (*((volatile uint32_t *)0x3FF4402C))

#define GPIO_OUT_W1TS_REG    (*((volatile uint32_t *)0x3FF44008))
#define GPIO_OUT_W1TC_REG    (*((volatile uint32_t *)0x3FF4400C))

#define GPIO_OUT1_W1TS_REG   (*((volatile uint32_t *)0x3FF44014))
#define GPIO_OUT1_W1TC_REG   (*((volatile uint32_t *)0x3FF44018))

#define GPIO_IN_REG          (*((volatile uint32_t *)0x3FF4403C))
#define GPIO_IN1_REG         (*((volatile uint32_t *)0x3FF44040))

// GPIO Matrix
#define GPIO_FUNC_OUT_SEL_CFG_REG_BASE 0x3FF44530
#define GPIO_FUNC_OUT_SEL_CFG_REG(gpio_num) (*((volatile uint32_t *)(GPIO_FUNC_OUT_SEL_CFG_REG_BASE + (gpio_num * 4))))
#define GPIO_FUNC_OUT_INV_SEL (1 << 10)
#define GPIO_FUNC_OEN_INV_SEL (1 << 9)
#define GPIO_FUNC_OEN_SEL (1 << 8)
#define GPIO_FUNC_OUT_SEL_M (0xFF)
#define GPIO_FUNC_OUT_SEL_S 0

// LEDC (PWM Controller)
#define LEDC_BASE 0x3FF5E000

// --- High Speed Timer Registers ---
#define LEDC_HS_TIMER_CONF_REG(timer_num) (*((volatile uint32_t *)(LEDC_BASE + 0x0000 + (timer_num * 0x20))))
#define LEDC_HS_TIMER_DUTY_RES_S 0
#define LEDC_HS_TIMER_CLK_DIV_S 4
#define LEDC_HS_TIMER_PAUSE (1 << 19)
#define LEDC_HS_TIMER_RST (1 << 20)
#define LEDC_HS_TIMER_TICK_SEL (1 << 21) // 0: REF_TICK, 1: APB_CLK (80MHz)
#define LEDC_HS_TIMER_PARA_UP (1 << 22)

// --- High Speed Channel Registers ---
#define LEDC_HS_CH_CONF0_REG(ch_num) (*((volatile uint32_t *)(LEDC_BASE + 0x0004 + (ch_num * 0x20))))
#define LEDC_HS_CH_TIMER_SEL_S 0
#define LEDC_HS_CH_TIMER_SEL_H 1
#define LEDC_HS_CH_SIG_OUT_EN (1 << 2)
#define LEDC_HS_CH_IDLE_LV (1 << 3)

#define LEDC_HS_CH_CONF1_REG(ch_num) (*((volatile uint32_t *)(LEDC_BASE + 0x0008 + (ch_num * 0x20))))
#define LEDC_HS_CH_DUTY_START (1 << 31)

#define LEDC_HS_CH_HPOINT_REG(ch_num) (*((volatile uint32_t *)(LEDC_BASE + 0x000C + (ch_num * 0x20))))
#define LEDC_HS_CH_HPOINT_S 0

#define LEDC_HS_CH_DUTY_REG(ch_num) (*((volatile uint32_t *)(LEDC_BASE + 0x0010 + (ch_num * 0x20))))
#define LEDC_HS_CH_DUTY_S 0

// --- LEDC Signal Output Mapping ---
#define LEDC_HS_SIG_OUT0 5
#define LEDC_HS_SIG_OUT1 6
#define LEDC_HS_SIG_OUT2 7
#define LEDC_HS_SIG_OUT3 8