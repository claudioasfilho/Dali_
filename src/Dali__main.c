#include <SI_EFM8UB1_Register_Enums.h>
#include "InitDevice.h"
#include "Dali.h"


//-----------------------------------------------------------------------------
// Pin Definitions
//-----------------------------------------------------------------------------

#define DISP_BC_DRIVEN   0             // 0 = Board Controller drives display
#define DISP_EFM8_DRIVEN 1             // 1 = EFM8 drives display

//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------
void main (void)
{
	uint8_t test=0;

	uint16_t fcounter;
   enter_DefaultMode_from_RESET();

  // DISP_EN = DISP_BC_DRIVEN;           		// EFM8 does not drive display

   IE_EA = 1;                          		// Enable global interrupts
   PRTDRV |=0x1;							//Enable high drive strength on P0

   SetDaliInputPinPolarity(ACTIVE_LOW);


   while (1) {
//	   DALI_OUT=1;
	   if (PB0_SW==0) {
		  // ManchesterEncoder(0xf0);
		  DaliTXStateMachine(0xff, 0x00);
		   for(fcounter=0; fcounter<0xfff; fcounter++);
	   }

   }


}
