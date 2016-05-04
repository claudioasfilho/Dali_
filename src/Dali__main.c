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
   xdata DALIRXREGISTERS DaliRead;
void main (void)
{
	uint8_t test=0;

	uint16_t fcounter;
   enter_DefaultMode_from_RESET();

   DisableDisplay();
  // DISP_EN = DISP_BC_DRIVEN;           		// EFM8 does not drive display

   TCON &= 0xF7; //Add this line
   IE_EA = 1; // Enable global interrupts

   IE_EA = 1;                          		// Enable global interrupts
   //PRTDRV |=0x1;							//Enable high drive strength on P0

  // SetDaliInputPinPolarity(ACTIVE_LOW);



   ClearDaliFlags();
   ClearDaliRXData();

   while (1) {

#ifdef DALIMASTERMODE


	   if (PB0_SW==0) {

		  DaliTXFrame(0xf0, 0x0f);
		   for(fcounter=0; fcounter<0xffff; fcounter++);
	   }

	   if (GetDaliDataReadyFlag()==1)
	   {

		   ClearDaliDataReadyFlag();
		   DaliRead=GetDaliRXData();
		   NOP();

	   }

#else


	   if (GetDaliDataReadyFlag()==1)
	   {
		   DaliRead=GetDaliRXData();
		   ClearDaliDataReadyFlag();
		   DaliAnswerFrame(0xff);

	   }

#endif
   }


}
