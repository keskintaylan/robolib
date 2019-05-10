/*
 * Effects of Increasing a Parameter Independently
+-----------+----------------+-----------+--------------+-------------------+-----------+
| Parameter | RiseTime       | Overshoot | SettlingTime | SteadyState Error | Stability |
+-----------+----------------+-----------+--------------+-------------------+-----------+
| Kp        | Decrease       | Increase  | Small Change | Decrease          | Degrade   |
| Ki        | Decrease       | Increase  | Increase     | Eliminate         | Degrade   |
| Kd        | Minor Change   | Decrease  | Decrease     | No effect         | Improve if Kd is small |
+-----------+----------------+-----------+--------------+-------------------+-----------+

 * Ziegler–Nichols Method
+ Increase Kp until output oscillates. This value is [Ku] and oscillation period is [Pu]
+--------------+--------+----------+--------+
| Control Type | Kp     | Ki       | Kd     |
+--------------+--------+----------+--------+
| P            | 0.50Ku |    -     |   -    |
| PI           | 0.45Ku | 1.2Kp/Pu |   -    |
| PID          | 0.60Ku |   2Kp/Pu | KpPu/8 |
+--------------+--------+----------+--------+

 * Manual Method
+---------------------------------------------------------------------------------------+
+ Increase Kp until output oscillates or is ok. Then set Kp to half of that value.      |
+ Increase Ki until any offset is corrected in sufficient time                          |
+ Increase Kd until the overshooting is minimized                                       |
+---------------------------------------------------------------------------------------+

*/

#include "stdint.h"

#define pidMax  (2000)
#define pidMin  (-2000)
#define pidBias 0
//#define dt      0.01  // iteration time(sec)
#define dt      10  // iteration time(ms)

// change the values according to project needs
// you can see above for reference
int32_t Kp = 500;   // proportional term
int32_t Ki = 10;    // integral term
int32_t Kd = 10;    // derivative term

//-----------------------------------------------------------------------------
// @ brief : calculate pid with given actual and desired value
//-----------------------------------------------------------------------------
int16_t CalculatePID(uint8_t desired_value, uint8_t actual_value)
{
    static int16_t errorLast = 0;   // last error
    static float error_i = 0;       // integral term error
    int16_t error_n;                // proportional term error
    int16_t error_d;                // derivative term error
    int16_t u = 0;                  // return value

    error_n = desired_value - actual_value;

    //error_i += error_n*dt;           // use this for unit of seconds
    error_i += (float)error_n*dt/1000; // use this for unit of ms (more performance)

    //error_d = (error_n - errorLast)/dt;    // use this for unit of seconds
    error_d = (error_n - errorLast)*1000/dt; // use this for unit of ms (more performance)
    errorLast = error_n;

    u = Kp*error_n + (float)Ki*error_i + Kd*error_d + pidBias;
    
    if(u > pidMax) u = pidMax;
    else if(u < pidMin) u = pidMin;

    return u/10; // return a value between -200 .. +200
}
