
# robolib
libraries for robot projects

# QTR

## Example
- **initialize ports :**
	
		gpioPortCfg_t g[8] =
		{
			  { lineS1_GPIO_Port , lineS1_Pin },
			  { lineS2_GPIO_Port , lineS2_Pin },
			  { lineS3_GPIO_Port , lineS3_Pin },
			  { lineS4_GPIO_Port , lineS4_Pin },
			  { lineS5_GPIO_Port , lineS5_Pin },
			  { lineS6_GPIO_Port , lineS6_Pin },
			  { lineS7_GPIO_Port , lineS7_Pin },
			  { lineS8_GPIO_Port , lineS8_Pin },
		};
		gpioPortCfg_t q_en = {lineS_en_GPIO_Port, lineS_en_Pin};
		qtr_init(g, &q_en);
		qtr_enable();
	
- call **CheckQtr()** in a timer(or call manually) with an enough interval(<10ms) 
- call **GetQtrPosition()** to check if senors are in a line or not
# MOTOR

## Example
- **initialize ports :**

		motor_t *motorLeft, *motorRight;
		
		motorLeft = motor_init(motorLeft1_GPIO_Port, motorLeft1_Pin, motorLeft2_GPIO_Port, motorLeft2_Pin, &TIM2->CCR3);
		motorRight = motor_init(motorRight1_GPIO_Port, motorRight1_Pin, motorRight2_GPIO_Port, motorRight2_Pin, &TIM3->CCR2);
- **usage :**

        if(motor_isSpeedLock(motorLeft) == false)
        {
            motor_setSpeed(motorLeft, (motor_getSpeed(motorLeft)*85UL)/100); // just slow down for caution
            motor_speedLock(motorLeft);
        }
