//=========================================================
// src/Interrupts.c: generated by Hardware Configurator
//
// This file will be regenerated when saving a document.
// leave the sections inside the "$[...]" comment tags alone
// or they will be overwritten!
//=========================================================

         
// USER INCLUDES			
#include <SI_EFM8UB1_Register_Enums.h>
#include "InitDevice.h"
#include "Dali.h"

//extern bit MDone;
//extern bit MOutput;

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
// Configurator set for HFOSC0/8
#define SYSCLK             3062000

//static xdata DALI_FRAME State = START;
//static xdata uint8_t bitCounter = 0;
//static xdata uint8_t daliRXOutput=0;
//bit _1stHalf;
//bit _2ndHalf;





//-----------------------------------------------------------------------------
// INT1_ISR
//-----------------------------------------------------------------------------
//
// TIMER0 ISR Content goes here. Remember to clear flag bits:
// TCON::TF0 (Timer 0 Overflow Flag)
//
// Here we process the Timer0 interrupt and toggle the LED when appropriate
//
//-----------------------------------------------------------------------------
SI_INTERRUPT (INT1_ISR, INT1_IRQn)
{

	DaliRXDecoding(1);

}


//-----------------------------------------------------------------------------
// Dali RX Timer TIMER1_ISR
//-----------------------------------------------------------------------------
//
// TIMER1 ISR Content goes here. Remember to clear flag bits:
// TCON::TF1 (Timer 0 Overflow Flag)
//
// Here we process the Timer dedicated to Dali RX
//
//-----------------------------------------------------------------------------
SI_INTERRUPT (TIMER1_ISR, TIMER1_IRQn)
{

	ToogleTestLed4();
	DaliRXDecoding(0);

}


//-----------------------------------------------------------------------------
// Dali TX Timer ISR (TIMER0)
//-----------------------------------------------------------------------------
//
// TIMER0 ISR Content goes here. Remember to clear flag bits:
// TCON::TF0 (Timer 0 Overflow Flag)
//
// Here we process the Timer0 interrupt and toggle the LED when appropriate
//
//-----------------------------------------------------------------------------
SI_INTERRUPT (TIMER0_ISR, TIMER0_IRQn)
{

	ReloadDaliTxTimer(TMH, TML);
	DaliTxHandler();
	IsDaliBusQuiet();

}


