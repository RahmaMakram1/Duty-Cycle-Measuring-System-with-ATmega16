/*
 *Mini_project_3.c
 *
 *  Created on: Jul 27, 2021
 *      Author: rahma
 */
#include "lcd.h"

uint8 edges = 0;                                      // represents the number of edges.
uint16 time_High = 0;                                      // represents T1 in the example.
uint16 time_Period = 0;                                    // represents T2 in the example.
uint16 timePeriodPlusHigh = 0;                            // represents T3 in the example.
uint16 time_Value=0;                                       // represents Capture Register in the example.

ISR(INT0_vect)
{
	time_Value=TCNT1;
	edges++;
	if(edges == 1)	            // first rising edge
	{
		TCNT1=0;
		CLEAR_BIT(MCUCR,ISC00);

	}
	else if(edges == 2)            // first falling edge
	{
		time_High=time_Value;          // the time stored represents T1 in the example.
		SET_BIT(MCUCR,ISC00);            // configure INT0 to detect rising edge
	}
	else if(edges==3)              // second rising edge
	{
		time_Period=time_Value;        // the time stored represents T2 in the example.
		CLEAR_BIT(MCUCR,ISC00);          // configure INT0 to detect falling edge
	}
	else if(edges == 4)            // second falling edge
	{
		timePeriodPlusHigh=time_Value;     // the time stored represents T3 in the example.
		SET_BIT(MCUCR,ISC00);                 // configure INT0 to detect rising edge

	}
}



void Timer1_init(void)
{
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);
	TCNT1 = 0;
	TCCR1B =(1<<CS10) | (1<<CS11);
}

void INT0_Init(void)
{
	MCUCR|=(1<<ISC00)|(1<<ISC01); 		// Trigger INT0 with the rising edge
	GICR|=(1<<INT0);
	DDRD  &= (~(1<<PD2));               // Configure INT0/PD2 as input pin
}

	int main(void)
	{
		uint32 duty_Cycle = 0;
		SREG |= (1<<7);

		LCD_init();
		INT0_Init();
		Timer1_init();
		while(1)
		{
			if(edges == 4)
			{
				TCCR1A = 0;
				TCCR1B = 0;
				TCNT1 = 0;
				SREG  &= ~(1<<7);
				edges = 0;                    // we start over the edge counter
				LCD_displayString("Duty Cycle : ");
				//duty cycle = ((T3-T2)/(T3-T1))*100
				duty_Cycle = (((float)(timePeriodPlusHigh-time_Period) / (timePeriodPlusHigh - time_High)) * 100);
				LCD_intgerToString(duty_Cycle);
				LCD_displayCharacter('%');
			}

		}
	}

