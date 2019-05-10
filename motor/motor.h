/*
 * motor.h
 *
 *  Created on: 15 Nis 2019
 *      Author: tkeskin
 */

#define MOTORS_COUNT  2

#ifndef MOTOR_MOTOR_H_
#define MOTOR_MOTOR_H_

#include "stdint.h"
#include "stdbool.h"

typedef enum
{
	FORWARD,
	REVERSE
}direction_t;

typedef struct
{
    bool inited;
    bool speedLock;
	uint8_t speed; // 0-100
	uint8_t speedLimiter; // x/100
	direction_t direction;

	volatile void *speedReg;
	void *portPositive;
	void *portNegative;
	uint32_t pinPositive;
	uint32_t pinNegative;
}motor_t;

motor_t *motor_init(void *portPos, uint32_t pinPos, void *portNeg, uint32_t pinNeg, volatile void *spdReg);
void motor_run(motor_t *m, direction_t dir);
void motor_runws(motor_t *m, direction_t dir, uint8_t spd);
void motor_runForMS(motor_t *m, int dir, int spd, int ms);
void motor_setSpeed(motor_t *m, uint8_t spd);
void motor_stop(motor_t *m);
void motor_break(motor_t *m);

//-----------------------------------------------------------------------------
// @ brief : sets a speed limiter of the motor
//           limits the speed of the motor by percentage
// @ param : motor object, limitval(percent 0-100)
//-----------------------------------------------------------------------------
static inline void motor_setSpeedLimiter(motor_t *m, uint8_t lim)
{
    m->speedLimiter = lim;
}

//-----------------------------------------------------------------------------
// @ brief : locks the speed of the motor
//           for now just sets a flag. you should control the flag manually
//           by call the function "motor_isSpeedLock"
//           in further enhancement you will not able to change the speed of
//           the motor if the lock is enabled.
// @ param : motor object
//-----------------------------------------------------------------------------
static inline void motor_speedLock(motor_t *m)
{
    m->speedLock = true;
}

//-----------------------------------------------------------------------------
// @ brief : unlocks the speed of the motor
//           for now just sets a flag. you should control the flag manually
// @ param : motor object
//-----------------------------------------------------------------------------
static inline void motor_speedUnlock(motor_t *m)
{
    m->speedLock = false;
}

//-----------------------------------------------------------------------------
// @ brief : returns if speedlock is enabled or not
// @ param : motor object
//-----------------------------------------------------------------------------
static inline bool motor_isSpeedLock(motor_t *m)
{
    return(m->speedLock);
}

//-----------------------------------------------------------------------------
// @ brief : returns the speed of the motor by percentage
// @ param : motor object
//-----------------------------------------------------------------------------
static inline uint8_t motor_getSpeed(motor_t *m)
{
    return m->speed;
}

//-----------------------------------------------------------------------------
// @ brief : returns if the motor is inited
// @ param : motor object
//-----------------------------------------------------------------------------
static inline bool motor_isInited(motor_t *m)
{
    return m->inited;
}

#endif /* MOTOR_MOTOR_H_ */
