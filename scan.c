#include "msp430.h"

// constant parameters:
#define MOTOR_PIN_1         BIT6                      // Motor pin on P1.6 : alt
#define MOTOR_PIN_2         BIT5                      // Motor pin on P2.5 : azm
#define TXD                 BIT2                      // TXD on P1.2
#define RXD                 BIT1                      // RXD on P1.1


// macro definitions:
// usage: e is an expression
#define azmto(e) old = azm; azm = (e); glide_servo(old, azm, 1);
#define altto(e) old = alt; alt = (e); glide_servo(old, alt, 2);



// wait for time t
// (approximate)
void wait(unsigned long t)
{
  volatile unsigned long count = t;
  while (count)
    count--;
}



// given an int x, output x
void output(int x) {
  unsigned char TXByte;
  TXByte = (unsigned char)((x >> 8) & 0xFF);
  while (! (IFG2 & UCA0TXIFG)); // wait for TX buffer to be ready for new data
  UCA0TXBUF = TXByte;
  TXByte = (unsigned char)(x & 0xFF);
  while (! (IFG2 & UCA0TXIFG)); // wait for TX buffer to be ready for new data
  UCA0TXBUF = TXByte;
}

// initialization function:
void init_output() {
  P1DIR |= TXD;
  P1OUT |= TXD;   // initially set TXD to high for some reason

  /* Configure hardware UART */
  P1SEL = BIT1 + BIT2 ; // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2 ; // P1.1 = RXD, P1.2=TXD
  UCA0CTL1 |= UCSSEL_2; // Use SMCLK
  UCA0BR0 = 104; // Set baud rate to 9600 with 1MHz clock (Data Sheet 15.3.13)
  UCA0BR1 = 0;   // Set baud rate to 9600 with 1MHz clock
  UCA0MCTL = UCBRS0; // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST; // Initialize USCI state machine
}



// set the servo PWM to a given value
void set_servo(int pwm, int n) {
  switch (n) {
    case 1:
      CCR1 = pwm;
      break;
    case 2:
      CCR2 = pwm;
      break;
  }
}

// initialization function for servo stuff
void init_servo() {
  P1DIR |= MOTOR_PIN_1; 		  // set motor pins to output
  P1SEL |= MOTOR_PIN_1; 			// set motor pins to TA output 1
  
  P2DIR |= MOTOR_PIN_2; 		  // set motor pins to output
  P2SEL |= MOTOR_PIN_2; 			// set motor pins to TA output 2
  
  /* next three lines to use internal calibrated 1MHz clock: */
  BCSCTL1 = CALBC1_1MHZ;                    // Set range
  DCOCTL = CALDCO_1MHZ;
  BCSCTL2 &= ~(DIVS_3);                     // SMCLK = DCO = 1MHz
  
  // set timer period
  CCR0 = 10000;
  
  CCTL1 = OUTMOD_7;			// CCR1 reset/set
  CCR1 = 1000; 				  // CCR1 PWM duty cycle
  
  CCTL2 = OUTMOD_7;     // CCR1 reset/set
  CCR2 = 1000;          // CCR1 PWM duty cycle
  
  TACTL = TASSEL_2 + MC_1; 		// SMCLK, up mode
}

// gently glide a servo motor from one value to another:
void glide_servo(int start, int end, int n) {
    int step;
    int i;
    if (start > end)
        step = -1;
    else
        step = 1;
    for (i = start; i != end; i += step) {
        set_servo(i, n);
        wait(100);
    }
    set_servo(end, n);
}



void main(void)
{
  /* initialization: */
  WDTCTL = WDTPW + WDTHOLD; 		// Stop WDT
  // initialize P1 direction
  P1DIR = 0x00;                 // clear P1 direction
  init_output();    // we put this one first because it kills the servo init if we put it after for some reason
  init_servo();
  
  int old; // a variable used to temporarily store old values
  // intialize altitue and azimuth
  int alt = 1000; //altto(alt);
  int azm = 1000; //azmto(azm);

  
  wait(100000);
  azmto(1200);
  wait(100000);
  azmto(1000);
  while(1); // loop forever
}
