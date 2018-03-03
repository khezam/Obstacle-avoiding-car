#include "TM4C123.h"                    // Device header
#include <stdint.h>
#include <stdbool.h>
#include "C:\ti\TivaWare_C_Series-2.1.4.178\inc\tm4c123gh6pm.h"
#include "C:\ti\TivaWare_C_Series-2.1.4.178\driverlib\sysctl.h"
#include "C:\ti\TivaWare_C_Series-2.1.4.178\driverlib\interrupt.h"
#include "C:\ti\TivaWare_C_Series-2.1.4.178\driverlib\timer.h"
#include "C:\ti\TivaWare_C_Series-2.1.4.178\driverlib\pwm.h"



 uint32_t d1 ,d2,d3;

void delay_Microsecond(uint32_t time);
void Portinit(void);
void Timer0A_init(void);
uint32_t Timer0A_Handler(void);
void Timer0B_init(void);
uint32_t Timer0B_Handler(void);
void Timer2A_Handler(void);
void IntGlobalEnable(void);
void Timer2A_init(void);
void delayMs(int n);
uint32_t d1 = 0;
uint32_t d2 = 0;

int pw = 0;
int pw1 =0;
const double _16MHz_1clock = 62.5e-9; /*Value of 1clock cycle in nanoseconds*/
const uint32_t MULTIPLIER  = 5882;  /*Derived from speed of sound*/
 
#define ECHO 0x40 //PB6
#define ECHO1 0x80 //PB7
#define TRIGGER 0x10 //PA4 (OUTPUT)
#define TRIGGER1 0x40 //PA 6 (OUTPUT)
#define BLUE_LED 0x04 //PF2 onboard Blue LED 
#define RED_LED 0x02 //PF1 onboard RED LED
#define GREEN_LED 0x08//PF3

uint32_t highEdge, lowEdge;
uint32_t highEdge1,lowEdge1;
uint32_t ddistance; /*Distance in centimeters*/
uint32_t ddistance1;/*Distance in centimeters*/

uint32_t Timer0A_Handler(void) 
{ 
// sending trigger signal
    GPIO_PORTA_DATA_R &=~TRIGGER;   
    delay_Microsecond(12);
    GPIO_PORTA_DATA_R |=ECHO|TRIGGER;
    delay_Microsecond(12);
    GPIO_PORTA_DATA_R &=~TRIGGER;
   
    TIMER0_ICR_R =4;
    while((TIMER0_RIS_R & 4) ==0) {}; //Wait till captured
     // Detecting higher edge
    highEdge = TIMER0_TAR_R;
    TIMER0_ICR_R =4; //clear timer capture flag
    while((TIMER0_RIS_R & 4) ==0) {}; //Wait till captured
    // Detecting lower edge
    lowEdge = TIMER0_TAR_R;
   ddistance = lowEdge -highEdge;  //Calculating distance
    ddistance = _16MHz_1clock *(double) MULTIPLIER *(double)ddistance;
    return ddistance;
 
}

uint32_t Timer0B_Handler(void) {
//sending trigger signal 
    GPIO_PORTA_DATA_R &=~TRIGGER1;
    delay_Microsecond(12);
    GPIO_PORTA_DATA_R |= ECHO1|TRIGGER1;
    delay_Microsecond(12);
    GPIO_PORTA_DATA_R &=~TRIGGER1;
    TIMER0_ICR_R |=0x400;//clears the timer before read
    while((TIMER0_RIS_R & 0x400) ==0){}; //Wait till captured
   //receiving high edge signal
    highEdge1 = TIMER0_TBR_R;//reads the value from timer
    TIMER0_ICR_R |=0x400; //clear timer capture flag
    while((TIMER0_RIS_R & 0x400)  ==0){};
  //receiving low edge signal
    lowEdge1 = TIMER0_TBR_R;
    ddistance1 = lowEdge1 -highEdge1;
    ddistance1 = _16MHz_1clock *(double) MULTIPLIER *(double)ddistance1;
    return ddistance1;
}

void Timer2A_Handler(void) 
	{ 
 TIMER2_ICR_R |= 0x001;  // clear the interrupt flag
	
  d1 = Timer0A_Handler();
d2 = Timer0B_Handler();
// when both the USS receive obstacle
   if(d1 < 15&& d2 < 15) 
		{
    GPIO_PORTF_DATA_R |=BLUE_LED;
   GPIO_PORTF_DATA_R |=RED_LED;
   GPIO_PORTD_DATA_R |= 0x0C;  // it shoud be C0
 //both motors take reverse			
    delayMs(500);
      GPIO_PORTD_DATA_R &=~ 0x0C;
      GPIO_PORTD_DATA_R |= 0x02;
    // motor takes a U turn
     delayMs(250);
  }
  else 
  {
GPIO_PORTF_DATA_R &=~BLUE_LED;
GPIO_PORTF_DATA_R &=~RED_LED; 
}
// When One USS detects interrupt		

  if(d1 < 15) 
	{
           GPIO_PORTF_DATA_R |=BLUE_LED;
		      
		      GPIO_PORTD_DATA_R &=~0x0F;
           GPIO_PORTD_DATA_R |= 0x01;
          
        // robot takes a right turn
         delayMs(1000);
        GPIO_PORTD_DATA_R &=~ 0x01;
	
        }
	     
        else {
	GPIO_PORTF_DATA_R &=~BLUE_LED;
                 }
   //   when one of the USS detects obstacle
     
    if(d2 < 15) 
    {
    GPIO_PORTF_DATA_R |=RED_LED;
			 
			GPIO_PORTD_DATA_R &=~0x0F;
   GPIO_PORTD_DATA_R |= 0x02;
   delayMs(1000);
 GPIO_PORTD_DATA_R &=~ 0x02;
			
}
else 
GPIO_PORTF_DATA_R &=~RED_LED;


 
} 

void IntGlobalEnable(void)
{
    __asm("    cpsie   i\n");  // interrupt enable
}

void Portinit(void)
{
SYSCTL_RCGCPWM_R |= 0x02;        // enable clock to PWM1
SYSCTL_RCGCGPIO_R |= 0x20;       // enable clock to GPIOF
SYSCTL_RCGCGPIO_R |= 0x10;       // enable clock to GPIOE
SYSCTL_RCGCGPIO_R |= 0x08;       // enable clock to GPIOd
GPIO_PORTF_LOCK_R = 0x4C4F434B;
GPIO_PORTF_CR_R |= 0x01;
SYSCTL_RCC_R &= ~0x00100000;     // use system clock for PWM

GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock PF0
GPIO_PORTF_CR_R |= 0x01;
	
 GPIO_PORTF_DIR_R |= 0x09;            // set PORTF 3 pins as output (LED) pin
 GPIO_PORTF_DEN_R |= 0x09;             // set PORTF 3 pins as digital pins
GPIO_PORTD_DIR_R |= 0x0F;
GPIO_PORTD_DEN_R |= 0x0F;             // PORTB 3 as digital pins
SYSCTL_RCGCGPIO_R |=(1U<<0); //Enable clock for PORTA 
 GPIO_PORTA_DIR_R |=TRIGGER;
 GPIO_PORTA_DIR_R |=TRIGGER1;
GPIO_PORTF_DIR_R |=BLUE_LED;
GPIO_PORTF_DIR_R |=RED_LED;
 GPIO_PORTA_DEN_R |=(ECHO)|(TRIGGER);
GPIO_PORTA_DEN_R |= (ECHO1)|(TRIGGER1);
GPIO_PORTF_DEN_R |= BLUE_LED;
GPIO_PORTF_DEN_R |=RED_LED;
GPIO_PORTF_DIR_R |=GREEN_LED;

}

// main code 
int main(void){
	Portinit();
Timer0A_init();
 Timer0B_init();
 Timer2A_init();
 IntGlobalEnable();
while(1)
 {
// Robot moves in forward direction
GPIO_PORTD_DATA_R |= 0x03;
	 
	GPIO_PORTF_DATA_R |= 0x08;
	 

 }
 }
void delay_Microsecond(uint32_t time)
{
    int i;
    SYSCTL_RCGCTIMER_R |=(1U<<1); 
    TIMER1_CTL_R=0;
    TIMER1_CFG_R=0x04;
    TIMER1_TAMR_R=0x02;
    TIMER1_TAILR_R= 16-1;
    TIMER1_ICR_R =0x1;
    TIMER1_CTL_R |=0x01;
 
    for(i=0;i<time;i++){ 
    while((TIMER1_RIS_R & 0x1)==0)
    TIMER1_ICR_R = 0x1;
    }
 
}

void Timer0A_init(void)
{
// the configuratio of TIMER tells us if we want use CCP
// Using GPIO_PORTB as a Capture mode. Take a look at page 720 and 721 to see where all this info came from
// take a look at page 1340 to see corresponding of TIMER0 CCP	
	
    SYSCTL_RCGCTIMER_R |=(1U<<0); 		// Enabling the Timer clock in the run module of TIMER page 337-340		
    SYSCTL_RCGCGPIO_R |=(1U<<1); 			//enabling PORTB clock in the run module page 338-340				
    GPIO_PORTB_DIR_R &=~ECHO; 				// clearing the input which is ECHO				
    GPIO_PORTB_DEN_R |=ECHO; 					// enabling digital function of the data pins				
    GPIO_PORTB_AFSEL_R |=ECHO; 				// setting ECHO as an input and enabling alternate function				
    GPIO_PORTB_PCTL_R &=~0x0F000000;   // clearing the bits/field 25-27 page 687(port mux control 6 or PMC6)         
    GPIO_PORTB_PCTL_R |= 0x07000000;		// enabling digital function (GPIO PCTL PMC6 bit field encoding) 1346
    NVIC_PRI4_R &= ~0xE0000000;			// configuring priority register TIMER0A
// the output trigger of GPTMCTL register can be used to intiate transfers on the ADC module 	
    TIMER0_CTL_R &=~0x0001;    // clearing TIMER0A on the register TAEN page 735-738
// The register GPTMCFG configures the global operation of GPTM module and determine whether its 32bits or 16bits		
    TIMER0_CFG_R =4;   				//16 bit Timer configuration and this function is controlled 1:0 of GPTMTAMR
// This register TAMR controls the modes for TIMER0A when its used individually and together A and B	
    TIMER0_TAMR_R = 0x17;   // Timer count up by TACDIR bit. Edge time mode TACMR CAPTURE MODE TAMR of the bit 1:0 and bit 2 TAMR.page 233, 234				
    TIMER0_CTL_R |=0x00C;    	// Since  it can trigger an output trigger of ADC, we are enabling both edges rising and falling. bit 3:2								
    TIMER0_CTL_R |=0x0001;     // enabling GTPMCTL regsiter page 735 and 736  							
}



void Timer0B_init(void)
{
// the configruation of Timer tells if you want to use CCP	
// Using GPIO PORTB  as capture mode, take a look at pg 720 and 721 to see where all this info came from
// take a look at page 1340 to see the corresponding of TIMER0 CCP	
   // SYSCTL->RCGCTIMER |=(1U<<0); 
   // SYSCTL->RCGCGPIO |=(1U<<1); //PortB
    GPIO_PORTB_DIR_R &=~ECHO1; 							// clearing the input which is ECHO1
    GPIO_PORTB_DEN_R |=ECHO1; 							// enabling the digital function data pin
    GPIO_PORTB_AFSEL_R |=ECHO1;							//setting ECHO1 as an input
    GPIO_PORTB_PCTL_R &=~0xF0000000;        // clearing the bits/filed 28-31 page 687(port mux contro 7 OR PMC7)    
    GPIO_PORTB_PCTL_R |= 0x70000000;				//enabling digital function(GPIO PCTL PMC7 bit filed encoding ) page1346
  NVIC_PRI5_R |=0x00000020;						// configuring the proiority register on NVIC
// the output trigger of GPTM register can be used to initiate transfers on the ADC  module 	
    TIMER0_CTL_R &=~0x100;           // clearing TIMER0B on the register TBEN page 735 and 736
// The register GPTMCFG configures the global operation of GPTM module and determine whether its 32bits or 16bits	
	TIMER0_CFG_R =4;   							// 16 bit Timer configuration and this function is controlled by bits 1:0 of GPTMTBMR
// This register controls the modes for TimerB when its used indidually and together A and B	
	TIMER0_TBMR_R = 0x17;    // count up by TBCDIR bit. since im using capture mode of the bit 1:0  and bit 2 TBCMR. Page 233,234
	TIMER0_CTL_R |=0xC00;    // since it can trigger an output trigger of ADC, we are enabling both edges faling and rising of bit TBEVENT 11:10 
    TIMER0_CTL_R |=0x0100;      // enabling GPTMCTL register page 735 and 736
}


void Timer2A_init(void)
{
    SYSCTL_RCGCTIMER_R |=(1U<<2); 
   // NVIC_PRI4_R &= ~0xE0000000;
   TIMER2_CTL_R &=~0x001;           // TAen is disabed
    	TIMER2_CFG_R =4;   						//16 bit timer
    	TIMER2_TAMR_R = 0x02;    		// Timer PERIODIC
	TIMER2_TAILR_R = 7999999;				// counting down HALF SEC 
	NVIC_PRI5_R |=0x20000000;
	NVIC_EN0_R |= 0x00800000; 			// Enabling Timer2A
	TIMER2_IMR_R|= 0x001;  					//arming Timer2A
    	TIMER2_CTL_R |=0x0001;      // TAen enable to configure CFG
}


void delayMs(int n)
{
    int i, j;
    for(i = 0 ; i < n; i++)

    for(j = 0; j < 3180; j++)
            {}  // do nothing for 1 ms
}

