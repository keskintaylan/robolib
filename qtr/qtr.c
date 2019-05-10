/*
 * qtr.c
 *
 *  Created on: 16 Nis 2019
 *      Author: tkeskin
 */

#include "qtr.h"
#include "hw_abstract.h"

qtr_t qtr;

//-----------------------------------------------------------------------------
// @ brief : call it once to initialize sensors port and parameters
//-----------------------------------------------------------------------------
void qtr_init(gpioPortCfg_t *g, gpioPortCfg_t *enable)
{
    uint8_t i = 0;
    for(; i < NUMBER_OF_QTR_SENSORS; i++)
    {
        qtr.sensor[i].port = g->port;
        qtr.sensor[i].pin = g->pin;
        qtr.sensor[i].onTime = 0;
        qtr.sensor[i].state = QTR_SENSOR_STATE_NA;
        g++;
    }
    qtr.sensor_enable.port = enable->port;
    qtr.sensor_enable.pin = enable->pin;

    qtr.position = QTR_POSITION_NA;
    qtr.coarsePosition = QTR_POSITION_NA;
    qtr.refMinOnTime = MIN_ON_TIME_OF_QTR;
    qtr.refMaxOnTime = MAX_ON_TIME_OF_QTR;
    uint16_t hysteresisValMid = (qtr.refMaxOnTime + qtr.refMinOnTime) / 2;
    qtr.hysteresisValMin = hysteresisValMid - hysteresisValMid % 8;
    qtr.hysteresisValMax = hysteresisValMid + hysteresisValMid % 8;
    qtr.state = QTR_STATE_CHARGE;

    qtr.inited = true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void qtr_enable()
{
	setGpio(qtr.sensor_enable.port, qtr.sensor_enable.pin);
}
void qtr_disable()
{
	resetGpio(qtr.sensor_enable.port, qtr.sensor_enable.pin);
}

//-----------------------------------------------------------------------------
// @brief : resolves sensor states and calculate position of the nose
//-----------------------------------------------------------------------------
void setSensorState()
{
    uint8_t i = 0;
    for(; i < NUMBER_OF_QTR_SENSORS; i++)
    {
        if(qtr.sensor[i].onTime < qtr.hysteresisValMin)
        {
            qtr.sensor[i].state = QTR_SENSOR_STATE_WHITE;
        }
        else if(qtr.sensor[i].onTime > qtr.hysteresisValMax)
        {
            qtr.sensor[i].state = QTR_SENSOR_STATE_BLACK;
        }
        else if(qtr.sensor[i].state == QTR_SENSOR_STATE_NA)
        {
            if(i == 0) qtr.sensor[i].state = QTR_SENSOR_STATE_WHITE;
            else       qtr.sensor[i].state = qtr.sensor[i-1].state;
        }
    }
#ifdef SEARCH_FOR_BLACK
    // sensor :  0 1 2 3   4 5 6 7
    // coeff  : |4|3|2|1|-|1|2|3|4|

    // |4|3|2|1|-|1|2|3|4|
    //      * *   *			: 0 (mid)
    //    * * *   *         : 1 (few right)
    //    * *               : 2 (right)
    //  * *                 : 3 (far right)
    //  *                   : 4 (very right)
    uint8_t leftVal = 0;
	uint8_t rightVal = 0;
	uint8_t cntLeft = 0;
	uint8_t cntRight = 0;
	for(i = 0; i < NUMBER_OF_QTR_SENSORS/2; i++)
    {
    	if(qtr.sensor[i].state == QTR_SENSOR_STATE_BLACK)
    	{
    		leftVal += NUMBER_OF_QTR_SENSORS/2 - i; // sum of left sensor coeffs
    		cntLeft++;
    	}
    }
	leftVal /= cntLeft; // calculate average

    for(i = NUMBER_OF_QTR_SENSORS/2; i < NUMBER_OF_QTR_SENSORS; i++)
    {
    	if(qtr.sensor[i].state == QTR_SENSOR_STATE_BLACK)
    	{
    		rightVal += i - NUMBER_OF_QTR_SENSORS/2 + 1; // sum of right sensor coeffs
    		cntRight++;
    	}
    }
    rightVal /= cntRight; // calculate average

    int sens_val = leftVal - rightVal;

    if((cntLeft == NUMBER_OF_QTR_SENSORS/2 && cntRight == NUMBER_OF_QTR_SENSORS/2) || (cntLeft == 0 && cntRight == 0))
    {
        qtr.position = QTR_POSITION_NA;
    }
    else
    {
        qtr.position = (qtr_position_t)(sens_val + QTR_POSITION_MIDDLE);
    }
#else
#error "no settings for white surface"
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if 0
#define QTR_TMR_INV  10
void qtr_us_tmr()
{
    static uint16_t qtr_timer = 0;
    qtr_timer += QTR_TMR_INV;

    switch(qtr.state)
    {
        case QTR_STATE_CHARGE:
        {
            uint8_t i = 0;
            for(; i < NUMBER_OF_QTR_SENSORS; i++)
            {
                qtr.sensor[i].onTime = 0;
                setGpio(qtr.sensor[i].port, qtr.sensor[i].pin);
                setGpioOutputMode(qtr.sensor[i].port, qtr.sensor[i].pin);
            }
            qtr_timer = 0;
            qtr.state = QTR_STATE_STOPCHARGING;
        }
        break;

        case QTR_STATE_STOPCHARGING:
        {
            if(qtr_timer >= 12) // charge at least 10us
            {
                uint8_t i = 0;
                for(; i < NUMBER_OF_QTR_SENSORS; i++) setGpioInputMode(qtr.sensor[i].port, qtr.sensor[i].pin);
                qtr_timer = 0;
                qtr.state = QTR_STATE_DISCHARGE;
            }
        }
        break;

        case QTR_STATE_DISCHARGE:
        {
            uint8_t sensorSetCnt = 0; // prevent redundant wait
            if(++qtr_timer < 2000) // wait for 2 ms
            {
                uint8_t i = 0;
                for(; i < NUMBER_OF_QTR_SENSORS; i++)
                {
                    if(getGpio(qtr.sensor[i].port, qtr.sensor[i].pin))
                    {
                        qtr.sensor[i].onTime += QTR_TMR_INV;
                        sensorSetCnt++;
                    }
                }
            }
            if(qtr_timer >= 2000 || sensorSetCnt == 0) // 2ms wait ended or all sensors are already set
            {
                setSensorState();
                qtr.state = QTR_STATE_CHARGE;
            }
        }
        break;

        default :
        {
            qtr.state = QTR_STATE_CHARGE;
        }
        break;
    }
}
#endif

//-----------------------------------------------------------------------------
// @ brief : qtr oneshot calculation
//           calculates all sensors time values and set state of the nose
//           call it from isr or from a loop with disable interrupt and some interval
//-----------------------------------------------------------------------------
void CheckQtr()
{
    if(qtr.inited)
    {
        uint16_t i, j;

        // QTR_STATE_CHARGE:
        for(i = 0; i < NUMBER_OF_QTR_SENSORS; i++)
        {
            qtr.sensor[i].onTime = 0;
            setGpio(qtr.sensor[i].port, qtr.sensor[i].pin);
            setGpioOutputMode(qtr.sensor[i].port, qtr.sensor[i].pin);
        }
        delay_us(25);

        // QTR_STATE_STOPCHARGING:
        for(i = 0; i < NUMBER_OF_QTR_SENSORS; i++) // 430cycle
        {
            setGpioInputMode(qtr.sensor[i].port, qtr.sensor[i].pin);
        }

        // QTR_STATE_DISCHARGE:
        j = 200;
        while(--j > 0) // wait enough
        {
            //delay_us(1);
            for(i = 0; i < NUMBER_OF_QTR_SENSORS; i++) // 166-174 cycle
            {
                if(getGpio(qtr.sensor[i].port, qtr.sensor[i].pin))
                {
                    qtr.sensor[i].onTime += 1;
                }
            }
        }
        setSensorState();
    }
}

//-----------------------------------------------------------------------------
// @ brief : call it in a loop to calculate qtr positions
//           it calls CheckQtr with an interval
//-----------------------------------------------------------------------------
void CheckQtrManualInterval()
{
    static uint32_t qtrRefreshTmr = 0;

    if(millis() - qtrRefreshTmr > 4)
    {
        qtrRefreshTmr = millis();
        __disable_irq(); // to be ensure noone effect the timing calculation
        CheckQtr();
        __enable_irq();
    }
}

//-----------------------------------------------------------------------------
// @ brief : returns average position of the all sensors (state of the nose)
//-----------------------------------------------------------------------------
qtr_position_t GetQtrPosition()
{
    return qtr.position;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool IsQtrAllBlack()
{
    uint8_t i = 0;
    for(; i < NUMBER_OF_QTR_SENSORS; i++)
    {
        if(qtr.sensor[i].state == QTR_SENSOR_STATE_WHITE) break;
    }

    return i == NUMBER_OF_QTR_SENSORS;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool IsQtrAllWhite()
{
    uint8_t i = 0;
    for(; i < NUMBER_OF_QTR_SENSORS; i++)
    {
        if(qtr.sensor[i].state == QTR_SENSOR_STATE_BLACK) break;
    }

    return i == NUMBER_OF_QTR_SENSORS;
}
