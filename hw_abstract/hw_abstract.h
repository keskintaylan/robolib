#ifndef HW_ABSTRACT_H_
#define HW_ABSTRACT_H_

#include "main.h"
#include "dwt_stm32_delay.h"
#include "stdint.h"
#include "stddef.h"

#define SystemTick   (SysTick->VAL)
#define CycleCounter (DWT->CYCCNT)

#define TIM_INTERVAL 	100             // [ms] must be given according to pwm timer interval
#define MOTOR_SPEED(x)	(x * (TIM_INTERVAL / 100))


// @ brief : prototype from a ms counter
//           must be called in a timer and counts millisecond
uint32_t millis();

static volatile uint32_t ccnt = 0;
static volatile uint32_t ccntDiff = 0;
static inline void startCycleCounter()
{
    ccnt = CycleCounter;
}
static inline uint32_t getCycleCounterDiff()
{
    ccntDiff = CycleCounter - ccnt;
    return ccntDiff;
}

static inline void setGpioOutputMode(void *port, uint32_t pin)
{
	LL_GPIO_SetPinMode(port, pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(port, pin, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(port, pin, LL_GPIO_OUTPUT_PUSHPULL);
}

static inline void setGpioInputMode(void *port, uint32_t pin)
{
	LL_GPIO_SetPinMode(port, pin, LL_GPIO_MODE_FLOATING);
}

static inline uint32_t getGpio(void *port, uint32_t pin)
{
	return LL_GPIO_IsInputPinSet(port, pin);
}

static inline void setGpio(void *port, uint32_t pin)
{
	LL_GPIO_SetOutputPin(port, pin);
}

static inline void resetGpio(void *port, uint32_t pin)
{
	LL_GPIO_ResetOutputPin(port, pin);
}

static inline void setPwm(volatile void *reg, uint8_t ratio)
{
	*(uint32_t*)reg = MOTOR_SPEED(ratio);
}

#endif /* MOTOR_HW_ABSTRACT_H_ */
