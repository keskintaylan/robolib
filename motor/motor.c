/*
 * motor.c
 *
 *  Created on: 15 Nis 2019
 *      Author: tkeskin
 */

#include "hw_abstract.h"
#include "motor.h"

motor_t motor[MOTORS_COUNT];

//-----------------------------------------------------------------------------
// @brief : call it once to initialize motor port and parameters
//-----------------------------------------------------------------------------
motor_t *motor_init(void *portPos, uint32_t pinPos, void *portNeg, uint32_t pinNeg, volatile void *spdReg)
{
    static uint8_t i = 0;

    if(i < MOTORS_COUNT)
    {
        motor_t *m = &motor[i];
        m->inited = true;
        m->speed = 0;
        m->speedLimiter = 0;
        m->portPositive = portPos;
        m->pinPositive = pinPos;
        m->portNegative = portNeg;
        m->pinNegative = pinNeg;
        m->speedReg = spdReg;
        i++;
        return m;
    }
    return NULL;
}

//-----------------------------------------------------------------------------
// @ brief : starts the motor with direction parameter
// @ param : motor object, direction
//-----------------------------------------------------------------------------
void motor_run(motor_t *m, direction_t dir)
{
	m->direction = dir;
	if(m->direction == FORWARD)
	{
		setGpio(m->portPositive, m->pinPositive);
		resetGpio(m->portNegative, m->pinNegative);
	}
	else
	{
		resetGpio(m->portPositive, m->pinPositive);
		setGpio(m->portNegative, m->pinNegative);
	}
}

//-----------------------------------------------------------------------------
// @ brief : starts the motor with direction and speed parameters
// @ param : motor object, direction, speed(percent 0-100)
//-----------------------------------------------------------------------------
void motor_runws(motor_t *m, direction_t dir, uint8_t spd)
{
	motor_setSpeed(m, spd);

	m->direction = dir;
	if(m->direction == FORWARD)
	{
		setGpio(m->portPositive, m->pinPositive);
		resetGpio(m->portNegative, m->pinNegative);
	}
	else
	{
		resetGpio(m->portPositive, m->pinPositive);
		setGpio(m->portNegative, m->pinNegative);
	}
}

//-----------------------------------------------------------------------------
// @ brief : sets the motor speed by percentage
// @ param : motor object, speed(percent 0-100)
//-----------------------------------------------------------------------------
void motor_setSpeed(motor_t *m, uint8_t spd)
{
    if(spd > 100) spd = 100;
    m->speed = spd;
    if(m->speedLimiter) m->speed = ((uint16_t)m->speed * m->speedLimiter) / 100;
    setPwm(m->speedReg, m->speed);
}

//-----------------------------------------------------------------------------
// @ brief : stops the motor
// @ param : motor object
//-----------------------------------------------------------------------------
void motor_stop(motor_t *m)
{
	setPwm(m->speedReg, 0);
}

//-----------------------------------------------------------------------------
// @ brief : stops the motor with a break
// @ param : motor object
//-----------------------------------------------------------------------------
void motor_break(motor_t *m)
{
	resetGpio(m->portPositive, m->pinPositive);
	resetGpio(m->portNegative, m->pinNegative);
}
