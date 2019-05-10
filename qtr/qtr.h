/*
 * qtr.h
 *
 *  Created on: 16 Nis 2019
 *      Author: tkeskin
 */

#ifndef QTR_QTR_H_
#define QTR_QTR_H_

#include "stdint.h"
#include "stdbool.h"

#define SEARCH_FOR_BLACK            // line color that will be followed
#define NUMBER_OF_QTR_SENSORS   8   //

// these values are experimental
// set with an experiment or with a calibration process
#define MIN_ON_TIME_OF_QTR      60  // minimum time pass of pin set on a surface
#define MAX_ON_TIME_OF_QTR      250 // maximum time pass of pin set on a opposite surface
//--------------------------------------
//--------------------------------------
typedef enum
{
    QTR_STATE_CHARGE,
    QTR_STATE_STOPCHARGING,
    QTR_STATE_DISCHARGE,
}qtr_state_t;

typedef enum
{
    QTR_POSITION_NA,
    QTR_POSITION_VERY_LEFT = 1,
    QTR_POSITION_FAR_LEFT,
    QTR_POSITION_LEFT,
    QTR_POSITION_FEW_LEFT,
    QTR_POSITION_MIDDLE,
    QTR_POSITION_FEW_RIGHT,
    QTR_POSITION_RIGHT,
    QTR_POSITION_FAR_RIGHT,
    QTR_POSITION_VERY_RIGHT
}qtr_position_t;

typedef enum
{
    QTR_SENSOR_STATE_NA,
    QTR_SENSOR_STATE_WHITE,
    QTR_SENSOR_STATE_BLACK,
}qtr_sensor_state_t;

typedef struct
{
    void *port;
    uint32_t pin;
}gpioPortCfg_t;

typedef struct
{
    bool inited;
    qtr_state_t state;
    struct
    {
        void *port;
        uint32_t pin;
        uint16_t onTime;
        qtr_sensor_state_t state; // black-white
    }sensor[NUMBER_OF_QTR_SENSORS];

    gpioPortCfg_t sensor_enable;

    qtr_position_t position;  // how much left or right (1-9)
    qtr_position_t coarsePosition; // left-middle-right
    //qtr_position_t direction; // direction of the traction (left to right - right to left - middle)

    uint16_t refMinOnTime; // min discharge time(us) (white surface)
    uint16_t refMaxOnTime; // max discharge time(us) (black surface)
    uint16_t hysteresisValMin;
    uint16_t hysteresisValMax;
}qtr_t;
extern qtr_t qtr;

//---
void qtr_init(gpioPortCfg_t *g, gpioPortCfg_t *enable);
void qtr_enable();
void CheckQtr();
void CheckQtrManualInterval();
qtr_position_t GetQtrPosition();
bool IsQtrAllBlack();
bool IsQtrAllWhite();

#endif
