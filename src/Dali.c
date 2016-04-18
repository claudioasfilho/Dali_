//-----------------------------------------------------------------------------
// Dali.c
//-----------------------------------------------------------------------------
// Copyright 2014 Silicon Laboratories, Inc.
// http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
//
// Program Description:
//
// This program uses Timer0 in 8-bit counter/timer with reload mode.
// It uses the Timer0 to create an interrupt at a certain rate and toggles
// the LED when the interrupt counter reaches the selected value. 
//
// Resources:
//   SYSCLK - 24.5 MHz HFOSC0 / 8
//   Timer0 - 10 Hz interrupt
//   P1.4   - LED green
//   P2.3   - Display enable
//
//-----------------------------------------------------------------------------
// How To Test: EFM8UB1 STK
//-----------------------------------------------------------------------------
// 1) Place the switch in "AEM" mode.
// 2) Connect the EFM8UB1 STK board to a PC using a mini USB cable.
// 3) Compile and download code to the EFM8UB1 STK board.
//    In Simplicity Studio IDE, select Run -> Debug from the menu bar,
//    click the Debug button in the quick menu, or press F11.
// 4) Run the code.
//    In Simplicity Studio IDE, select Run -> Resume from the menu bar,
//    click the Resume button in the quick menu, or press F8.
// 5) Check that the green LED is blinking.
//
// Target:         EFM8UB1
// Tool chain:     Generic
//
// Release 0.1 (ST)
//    - Initial Revision
//    - 10 OCT 2014
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <SI_EFM8UB1_Register_Enums.h>
#include "InitDevice.h"
#include "Dali.h"




//bit MDone;
//bit MOutput;
//bit DaliStopFlag;
//bit DaliErrorFlag;

DALIFLAGS DaliFlags;

/*********************************************************************************
 *********************************************************************************
 	 	 	 	 	 Manchester Encoder Related Functions
 *********************************************************************************
 *********************************************************************************/


/*Manchester Decoder Busy Flag Getters and Setters*/

void SetBusyFlag()
{
	DaliFlags.flag.Busy = 1;
}

void ClearBusyFlag()
{
	DaliFlags.flag.Busy = 0;
}

bit GetBusyFlag()
{
 return DaliFlags.flag.Busy;
}

/*Manchester Decoder Output Flag Getters and Setters*/

void SetMDOutput()
{
	DaliFlags.flag.Output =1;
}

void ClearMDOutput()
{
	DaliFlags.flag.Output =0;
}

bit GetMDOutput()
{
 return DaliFlags.flag.Output;
}
SI_SBIT (LED2,SFR_P1, 4);			   //PB0 Switch Definition

void ManchesterEncoder (uint8_t input)
{
	static BITS_BYTE Input;

	static int8_t counter = 8;				//It uses 8 steps because it sends 7 bits and stills needs the time to conclude the last bit before it loads the counter again

	ClearBusyFlag();

	if (counter==8)
	{
		Input.Abyte = input;
	}

	while (counter>=0)
	{
		while(GetBusyFlag()==1);

		if (GetBusyFlag()==0)
		{
			switch (counter--){

							case 0: //SetBusyFlag();
							break;
							case 1: DaliFlags.flag.Output = Input.nybble.BB0;
							SetBusyFlag();
							break;
							case 2: DaliFlags.flag.Output = Input.nybble.BB1;
							SetBusyFlag();
							break;
							case 3: DaliFlags.flag.Output = Input.nybble.BB2;
							SetBusyFlag();
							break;
							case 4: DaliFlags.flag.Output = Input.nybble.BB3;
							SetBusyFlag();
							break;
							case 5: DaliFlags.flag.Output = Input.nybble.BB4;
							SetBusyFlag();
							break;
							case 6: DaliFlags.flag.Output = Input.nybble.BB5;
							SetBusyFlag();
							break;
							case 7: DaliFlags.flag.Output = Input.nybble.BB6;
							SetBusyFlag();
							break;
							case 8: DaliFlags.flag.Output = Input.nybble.BB7;
							SetBusyFlag();
							break;
				}


		}

	}

	counter=8;

}

/*********************************************************************************
 *********************************************************************************
 	 	 	 	 	 Dali TX Related Functions
 *********************************************************************************
 *********************************************************************************/

void DaliTxHandler()
{
	static uint8_t counter = 0;
	static uint8_t evcounter = 0;

	if (GetBusyFlag())
	{
		if (evcounter++==15)
			{evcounter=0;}
		if (GetDaliStopFlag()==0)		//Are these the Stop Bits?
		{

			//The next lines implement the two steps of the Manchester Decoding
			if (counter==0)					//Process First Part of the Byte
			{
				if (GetMDOutput()==0) SetDaliOutputPin();
				else ClearDaliOutputPin();
				counter++;
			}
			else
			{								//Process 2nd Part of the Byte

				if (GetMDOutput()==0) ClearDaliOutputPin();
				else SetDaliOutputPin();
				counter=0;

				ClearBusyFlag();		//Finished Processing Byte for Manchester Encoder
			}

		}
		else				//Yes, This is the Stop bits stage
			{
				if (counter++<4)	SetDaliOutputPin();		//It keeps the Output High for 4 Cycles of the Timer
				else{
						SetDaliOutputPin();		//The Line is normally High;
						counter=0;
						ClearDaliStopFlag();
						ClearBusyFlag();		//Finished Processing Byte for Manchester Encoder
					}
			}
		}

}


void ReloadDaliTxTimer(uint8_t reloadH, uint8_t reloadL)
{
	TH0 = reloadH;
	TL0 = reloadL;
}


void StartDaliTxTimer()
{

	TCON |= TCON_TR0__RUN;		//Enables Timer 1 Run
}


uint16_t GetDaliTxTimer()
{
	return TH0|TL0;
}


void StopDaliTxTimer()
{

	TH1 = 0;
	TL1 = 0;
	TCON |= TCON_TR0__STOP;		//Enables Timer 1 Run
}


void DaliFrameStart()
{
	ClearBusyFlag();				//Clear Flag that indicates the Manchester Decoder is Busy on the Interrupt
	SetMDOutput();
	SetBusyFlag();					//Sets the Busy Flag so the Bit can be processed
	while(GetBusyFlag()==1);		//Waits until the Bit gets transmitted

}

void DaliFrameStop()
{
	ClearBusyFlag();		//Clear Flag that indicates the Manchester Decoder is Busy on the Interrupt
	SetDaliStopFlag();
	SetBusyFlag();			//Sets the Busy Flag so the Bit can be processed
	while(GetBusyFlag()==1);		//Waits until the Bit gets transmitted

}

void DaliTXStateMachine(uint8_t address, uint8_t Ddata)
{
	DALI_TXFRAME States;

	SetDaliOutputPin();			//The Line is Normally High
	States = START;

	while (States<END)
	{
		switch (States)
		{
			case START: {
							DaliFrameStart();
							States= ADDRESS;
							break;
						}

			case ADDRESS: {
							ManchesterEncoder(address);
							States= DATA;
							break;
						}

			case DATA: {
							ManchesterEncoder(Ddata);
							States= STOP;
							break;
						}

			case STOP: {
							DaliFrameStop();
							States= END;
							break;
						}

		}
	}

}
/*Dali Stop bit Flag Getters and Setters*/

void SetDaliStopFlag()
{
	DaliFlags.flag.Stop =1;
}

void ClearDaliStopFlag()
{
	DaliFlags.flag.Stop =0;
}

bit GetDaliStopFlag()
{
 return DaliFlags.flag.Stop;
}


void SetDaliOutputPin()
{
	DALI_OUT=1;
}

void ClearDaliOutputPin()
{
	DALI_OUT=0;
}

bit GetDaliOutputPin()
{
 return DALI_OUT;
}


/*********************************************************************************
 *********************************************************************************
 	 	 	 	 	 Dali RX Related Functions
 *********************************************************************************
 *********************************************************************************/
uint8_t BusQuietCounter;

bit GetDaliIntputPin()
{
 return DALI_IN;
}

void EnableDaliRxTimerInt()
{
	IE |= IE_ET1__ENABLED;
}

void DisableDaliRxTimerInt()
{
	IE &= 0xf7;
}


void ReloadDaliRxTimer(uint8_t reloadH, uint8_t reloadL)
{
	TH1 = reloadH;
	TL1 = reloadL;
}

void StartDaliRxTimer()
{

	TCON |= TCON_TR1__RUN;		//Enables Timer 1 Run
}


uint16_t GetDaliRxTimer()
{
	return TH1|TL1;
}


void StopDaliRxTimer()
{

	TH1 = 0;
	TL1 = 0;
	TCON |= TCON_TR1__STOP;		//Enables Timer 1 Run
}

void QueryDaliBus()
{
	static counter=0;

	if ((GetDaliIntputPin()==1) && (counter++==1))
	{
		counter=0;
		if(GetBusQuietCounter()<=254) IncBusQuietCounter();
	}
	if (GetDaliIntputPin()==0)
	{
		counter=0;
		ClearBusQuietCounter();
	}
}


void IncBusQuietCounter()
{
	BusQuietCounter++;
}

void ClearBusQuietCounter()
{
	BusQuietCounter=0;
}

uint8_t GetBusQuietCounter()
{
	return BusQuietCounter;
}


void SetDaliRxErrorFlag()
{
	DaliFlags.flag.Error = 1;
}

void ClearDaliRxErrorFlag()
{
	DaliFlags.flag.Error = 0;
}

bit GetDaliRxErrorFlag()
{
	return DaliFlags.flag.Error;
}


void SetDaliInputPinPolarity (INTPOLARITY input)
{
	if (input==ACTIVE_HIGH) IT01CF |= 0x80;
	else IT01CF &=0x7f;
}


void EnableInt1 ()
{
	IE |= 0x4;
}

void DisableInt1 ()
{
	IE &= 0xfb;
}

