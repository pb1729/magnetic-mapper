#include "msp430.h"


// constant parameters:
#define MOTOR_PIN BIT2


// wait for time t
// (approximate)
void wait(unsigned long t)
{
  volatile unsigned long count = t;
  while (count)
    count--;
}


void init(void)
{
  WDTCTL = WDTPW + WDTHOLD; 		// Stop WDT
  
  // initialize P1 direction
  P1DIR = 0x00;                 // clear P1 direction
  P1DIR |= MOTOR_PIN; 				// set motor pin to output
  P1SEL |= MOTOR_PIN; 				// set motor pin to TA0.1
  
  /* next three lines to use internal calibrated 1MHz clock: */
  BCSCTL1 = CALBC1_1MHZ;                    // Set range
  DCOCTL = CALDCO_1MHZ;
  BCSCTL2 &= ~(DIVS_3);                     // SMCLK = DCO = 1MHz
  
  // set timer period
  CCR0 = 10000-1; 				// PWM Period is 3000
  
  CCTL1 = OUTMOD_7;			// CCR1 reset/set
  CCR1 = 600; 				// CCR1 PWM duty cycle
  
  TACTL = TASSEL_2 + MC_1; 		// SMCLK, up mode
}


void main(void)
{
  init();
  while (1) {
    wait(100000);
    CCR1 = 550;
    wait(100000);
    CCR1 = 2330;
  }
}
  
  
  
  
  
