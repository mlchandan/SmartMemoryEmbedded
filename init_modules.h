/* All Include files */

///////////Common Include Files////////
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_ints.h"
#include "inc/hw_timer.h"
#include "driverlib/gpio.h"
#include <time.h>
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/ssi.h"
///////////////////////////////////////

///////////For UART Module/////////////
#include "inc/hw_uart.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "utils/uartstdio.c"
/* Include files ends here */
/****************************************************************************************/

//----------------------------------------
// Prototypes
//----------------------------------------
void inituart (void);
void commoninit (void);

/****************************************************************************************/

//-----------------------
// 1) Common Initialisation
//-----------------------

void commoninit (void)
{

	//SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// Enable GPIO port A which is used for UART0 pins.

	// change this to whichever GPIO port you are using.

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

}

//-----------------------
// 2) inituart
//-----------------------

void inituart (void)
{

// Enable UART0 so that we can configure the clock.

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

// Configure the pin muxing for UART0 functions on port A0 and A1.

// This step is not necessary if your part does not support pin muxing.

// change this to select the port/pin you are using.

	GPIOPinConfigure(GPIO_PA0_U0RX);

	GPIOPinConfigure(GPIO_PA1_U0TX);

	// change this to select the port/pin you are using.

	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);



// Use the internal 16MHz oscillator as the UART clock source.

UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

// Select the alternate (UART) function for these pins.

// Initialize the UART for console I/O.

UARTStdioConfig(0, 115200, 16000000);

}


//------------------------------
// 3) PortF_init (Test Function)
//------------------------------
void PortF_Init(void){  unsigned long volatile delay;
  SYSCTL_RCGCGPIO_R |= 0x20; // activate port F
  delay = SYSCTL_RCGCGPIO_R;
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0
  GPIO_PORTF_CR_R |= 0x1F;           // allow changes to PF4-0
  GPIO_PORTF_DIR_R = 0x0E;     // make PF3-1 output (PF3-1 built-in LEDs),PF4,0 input
  GPIO_PORTF_PUR_R = 0x11;     // PF4,0 have pullup
  GPIO_PORTF_AFSEL_R = 0x00;   // disable alt funct on PF4-0
  GPIO_PORTF_DEN_R = 0x1F;     // enable digital I/O on PF4-0
  GPIO_PORTF_PCTL_R = 0x00000000;
  GPIO_PORTF_AMSEL_R = 0;      // disable analog functionality on PF
}
