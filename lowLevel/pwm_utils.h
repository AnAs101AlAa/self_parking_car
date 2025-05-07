#define GPIO_ENABLE_W1TS_REG (*(volatile uint32_t*)0x3FF44024)
#define GPIO_ENABLE1_W1TS_REG (*(volatile uint32_t*)0x3FF44030)

#define GPIO_FUNC0_OUT_SEL_CFG_REG 0x3FF44530
#define LEDC_CONF_REG (*(volatile uint32_t*)0x3FF59190)
#define DPORT_PERIP_CLK_EN_REG (*(volatile uint32_t*)0x3FF000C0)
#define DPORT_PERIP_RST_EN_REG (*(volatile uint32_t*)0x3FF000C4)
#define resolution (uint)8

volatile uint32_t* LEDC_HSCH_CONF0_REG[] = {
    (volatile uint32_t*)0x3FF59000,
    (volatile uint32_t*)0x3FF59014,
    (volatile uint32_t*)0x3FF59028,
    (volatile uint32_t*)0x3FF5903C
};

volatile uint32_t* LEDC_HSTIMER_CONF_REG[] = {
    (volatile uint32_t*)0x3FF59140,
    (volatile uint32_t*)0x3FF59148,
    (volatile uint32_t*)0x3FF59150,
    (volatile uint32_t*)0x3FF59158
};

volatile uint32_t* LEDC_HSCH_CONF1_REG[] = {
    (volatile uint32_t*)0x3FF5900C,
    (volatile uint32_t*)0x3FF59020,
    (volatile uint32_t*)0x3FF59034,
    (volatile uint32_t*)0x3FF59048
};

volatile uint32_t* LEDC_HSCH_DUTY_REG[] = {
    (volatile uint32_t*)0x3FF59008,
    (volatile uint32_t*)0x3FF5901C,
    (volatile uint32_t*)0x3FF59030,
    (volatile uint32_t*)0x3FF59044
};

volatile uint32_t* LEDC_HSCH_HPOINT_REG[] = {
    (volatile uint32_t*)0x3FF59004,
    (volatile uint32_t*)0x3FF59018,
    (volatile uint32_t*)0x3FF5902C,
    (volatile uint32_t*)0x3FF59040
};

volatile uint32_t* IO_MUX_GPIO_REG[] = {
    (volatile uint32_t*)0x3FF49028, //26
    (volatile uint32_t*)0x3FF4902C, //27
    (volatile uint32_t*)0x3FF49024, //25
    (volatile uint32_t*)0x3FF49020 //33
};

volatile uint32_t* LEDC_HSTIMER_VALUE_REG[] = {
    (volatile uint32_t*)0x3FF59144,
    (volatile uint32_t*)0x3FF5914C,
    (volatile uint32_t*)0x3FF59154,
    (volatile uint32_t*)0x3FF5915C
};


void motorsInit(int* motorPins) {
    DPORT_PERIP_CLK_EN_REG |= (1<<11);// enable clock for ledc
    uint divider = 80000000 / (5000 * 256);

    for(int i=0;i<4;i++) {
        *LEDC_HSTIMER_CONF_REG[i] &= ~(0xf);
        *LEDC_HSTIMER_CONF_REG[i] |= resolution; //resolution = 8 bit
        *LEDC_HSTIMER_CONF_REG[i] |= (divider<<13);

        *LEDC_HSCH_CONF0_REG[i] &= ~(0b00); //timer 0
        *LEDC_HSCH_CONF0_REG[i] |= (1<<2); //enale output channel

        *LEDC_HSCH_HPOINT_REG[i] = 1; // value to set high
        *LEDC_HSCH_DUTY_REG[i] &= ~(0xffffff);
        *LEDC_HSCH_DUTY_REG[i] |= (20<<4); // low duty cycle

        // gpio setting
        volatile uint32_t* gpio_cfg = (volatile uint32_t*)GPIO_FUNC0_OUT_SEL_CFG_REG + motorPins[i];
        // peripheral 71 -> hschan0
        *gpio_cfg = 71 + i;

        if(motorPins[i] < 32) {
            GPIO_ENABLE_W1TS_REG |= (1<<motorPins[i]);
        } else {
            GPIO_ENABLE1_W1TS_REG |= (1<<motorPins[i] - 32);
        }
        // function 2
        *IO_MUX_GPIO_REG[i] &= ~(0b111 << 12);
        *IO_MUX_GPIO_REG[i] |= (2<<12);

        *LEDC_HSCH_CONF1_REG[i] |= (1<<31); // start channel duty cycle
        *LEDC_HSTIMER_CONF_REG[i] &= ~(1<<24); //reset timer
    }
}

void motorOn(int index, int direction, int duty) {
    if(direction == 1) {
        *LEDC_HSCH_DUTY_REG[index] &= ~(0xffffff);
        *LEDC_HSCH_DUTY_REG[index] |= (duty<<4);
        *LEDC_HSCH_CONF1_REG[index] |= (1<<31); // start channel duty cycle
        *LEDC_HSCH_DUTY_REG[index+1] &= ~(0xffffff);
        *LEDC_HSCH_DUTY_REG[index+1] &= ~(0<<4);
        *LEDC_HSCH_CONF1_REG[index+1] |= (1<<31); // start channel duty cycle
    } else {
        *LEDC_HSCH_DUTY_REG[index] &= ~(0xffffff);
        *LEDC_HSCH_DUTY_REG[index] &= ~(0<<4);
        *LEDC_HSCH_CONF1_REG[index] |= (1<<31); // start channel duty cycle
        *LEDC_HSCH_DUTY_REG[index+1] &= ~(0xffffff);
        *LEDC_HSCH_DUTY_REG[index+1] |= (duty<<4);
        *LEDC_HSCH_CONF1_REG[index+1] |= (1<<31); // start channel duty cycle
    }
}

void motorOff(int index) {
    *LEDC_HSCH_DUTY_REG[index] &= ~(0xffffff);
    *LEDC_HSCH_DUTY_REG[index] &= ~(0<<4);
    *LEDC_HSCH_CONF1_REG[index] |= (1<<31); // start channel duty cycle
    *LEDC_HSCH_DUTY_REG[index+1] &= ~(0xffffff);
    *LEDC_HSCH_DUTY_REG[index+1] &= ~(0<<4);
    *LEDC_HSCH_CONF1_REG[index+1] |= (1<<31); // start channel duty cycle
}

void carForward(int duty) {
    motorOn(0, 0, duty + 6);
    motorOn(2, 1, duty);
}

void carBackward(int duty) {
    motorOn(0, 1, duty + 6);
    motorOn(2, 0, duty);
}

void carStop(){
    motorOff(0);
    motorOff(2);
}

void carSpinRight(int time) {
    motorOn(0, 1, 20);
    motorOn(2,0, 60);
    delay(time);
    carStop();
}

void carSpinLeft(int time) {
    motorOn(0, 1, 60);
    motorOn(2, 0, 20);
    delay(time);
    carStop();
}