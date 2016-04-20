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
	DALI_FRAME States;

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

bit _1stQ;
bit _2ndQ;
bit _3rdQ;
bit _4thQ;




DALI_FRAME startconditionbitDemodulation()
{
	static xdata DALI_DEMOD bitState = _1qB;

	switch (bitState)
	{
		case _1qB: //This is Triggered by the INT1_ISR
		{
			if((GetDaliIntputPin()==DALI_LOGIC_0))
			{

				//_1stQ = GetDaliIntputPin();
				ReloadnStartDaliRxTimer(STMH, STML);
				bitState = _2qB;
				//ToogleTestLed();
			}
			break;
		}

		case _2qB: //This is Triggered by the TIMER1_ISR
		{
			if((GetDaliIntputPin()==DALI_LOGIC_0))
			{

				//_2ndQ = GetDaliIntputPin();
				StopnDisableDaliRxTimer();
				SetDaliInputPinPolarity(ACTIVE_HIGH);
				EnableInt1();
				bitState = _3qB;
				//ToogleTestLed();
			}
			else
			{
				StopnDisableDaliRxTimer();
			bitState = _1qB;
			}
			break;
		}

		case _3qB: //This is Triggered by the TIMER1_ISR
		{
			if((GetDaliIntputPin()==DALI_LOGIC_1))
			{

				//_3rdQ = GetDaliIntputPin();
				ReloadnStartDaliRxTimer(STMH, STML);
				bitState = _4qB;
				//ToogleTestLed();

			}
			break;

		}
		case _4qB: //This is Triggered by the TIMER1_ISR, once completed successfully it disables the INT1 and it uses the Timer to check the DALI_IN State
		{
			if((GetDaliIntputPin()==DALI_LOGIC_1))
			{

				//_4thQ = GetDaliIntputPin();
				DisableInt1();
				ReloadnStartDaliRxTimer(STMH, STML);
				bitState = _1qB;
				//ToogleTestLed();
				return ADDRESS; //Processing is done. It resets this state machine and moves the main state machine to the next state
			}
			else
			{
				//Something is wrong with the Data, it resets the INT trigger and Disables Timer
				StopnDisableDaliRxTimer();
				SetDaliInputPinPolarity(ACTIVE_LOW);
				EnableInt1();
				StopnDisableDaliRxTimer();
				bitState = _1qB;
				return IDLE;
			}
			break;
		}

	}

	return START; //Still Processing the Bit
}




bit bitDemodulation()
{
	static xdata DALI_DEMOD bitState = _1qB;

	switch (bitState)
		{
			case _1qB: //This is Triggered by the TIMER1_ISR
			{

				_1stQ = GetDaliIntputPin();
				ReloadnStartDaliRxTimer(STMH, STML);
				bitState = _2qB;
				ToogleTestLed();
				break;
			}

			case _2qB: //This is Triggered by the TIMER1_ISR
			{
				_2ndQ = GetDaliIntputPin();
				ReloadnStartDaliRxTimer(STMH, STML);
				bitState = _3qB;
				ToogleTestLed();
				break;
			}

			case _3qB: //This is Triggered by the TIMER1_ISR
			{

				_3rdQ = GetDaliIntputPin();
				ReloadnStartDaliRxTimer(STMH, STML);
				bitState = _4qB;
				ToogleTestLed();
				break;

			}
			case _4qB: //This is Triggered by the TIMER1_ISR
			{

				_4thQ = GetDaliIntputPin();
				ReloadnStartDaliRxTimer(STMH, STML);
				bitState = _1qB;
				ToogleTestLed();
				return 1; //Processing is done. It resets this state machine and informs user the bit information is available
				break;
			}
		}
	return 0;	//Still Processing the Bit

}

//This Function returns the Logic level of the Bit read by the bitDemodulation() function and it clears the "Quarter" bits. If the bit is invalid, it returns -1
int isbitHighorLow()
{
	int output=-1;

	if ((_1stQ==DALI_LOGIC_0) && (_2ndQ==DALI_LOGIC_0) && (_3rdQ==DALI_LOGIC_1) && (_4thQ==DALI_LOGIC_1)) output = 1;
	else if ((_1stQ==DALI_LOGIC_1) && (_2ndQ==DALI_LOGIC_1) && (_3rdQ==DALI_LOGIC_0) && (_4thQ==DALI_LOGIC_0)) output = 0;

	_1stQ=0;
	_2ndQ=0;
	_3rdQ=0;
	_4thQ=0;

	return output;
}

int8_t ReadDaliByte()
{
	static xdata BITS_BYTE address;
	static xdata uint8_t bitscounter=7;
	int bitread=0;

	if (bitDemodulation())	//Is Demodulation done for the bit?
	{
		bitread = isbitHighorLow();

		if (bitread!=-1)
		{
			switch (bitscounter)
			{
				case 0: address.nybble.BB0 = bitread;
						break;

				case 1: address.nybble.BB1 = bitread;
						break;

				case 2: address.nybble.BB2 = bitread;
						break;

				case 3: address.nybble.BB3 = bitread;
						break;

				case 4: address.nybble.BB4 = bitread;
						break;

				case 5: address.nybble.BB5 = bitread;
						break;

				case 6: address.nybble.BB6 = bitread;
						break;

				case 7: address.nybble.BB7 = bitread;
						break;
			}
			if(bitscounter--==0)
			{
				NOP();
				bitscounter=7;
				return address.Abyte;
			}

		}

		else //If any of the bits is corrupted, the device goes to IDLE MODE
		{
			return -1;
		}

		return -2;		//returns -2 while collecting all the bits from the Dali Byte

	}
}

void DaliRXDecoding()
{
	static xdata DALI_FRAME State = IDLE;
	static xdata int8_t address;


	switch (State)

	   		{

				case IDLE:			//In this state, it checks if the RX bus was quite and also if it receives the start bit
		   					{
		   						if (GetBusQuietCounter()>1) State = START;
		   						address=0;
		   						//This Jumps straight to the next State on the State Machine so we don't loose a cycle
		   					}

				case START:			//In this state, it checks if the RX bus was quite and also if it receives the start bit
		   					{
		   						State = startconditionbitDemodulation();
		   						break;
		   					}

	   			case ADDRESS:			//In this state, it checks if the RX bus was quite and also if it receives the start bit
	   					{
	   						address= ReadDaliByte();

	   						if ((address!=-2)&&(address!=-1))	//Is Demodulation done for the bit?
	   						{
	   							NOP();

	   							}

	   							else if (address==-1) //If any of the bits is corrupted, the device goes to IDLE MODE
	   							{
	   								State = IDLE;
									#warning "Add_a_flag_indicating_error"
	   							}
	   						break;
	   					}

	   			case DATA:
	   					{

	   					}//End of case DATA:

	   			default: State=IDLE;

	   		}//End of Switch State:



#if 0

   static xdata uint8_t intcounter = 0;
   static xdata uint8_t debugcounter = 0;

   //if((GetDaliIntputPin()==0) &&(intcounter==0))
 	if((GetBusQuietCounter()>1)&&(GetDaliIntputPin()==0) &&(intcounter==0))
	{

		_1stHalf=GetDaliIntputPin();
		SetDaliInputPinPolarity(ACTIVE_HIGH);

		if(debugcounter++==9)
		{
			NOP();//ToogleTestLed();
		}


	}

	if ((intcounter++==1))
	{
		_2ndHalf=GetDaliIntputPin();
		SetDaliInputPinPolarity(ACTIVE_LOW);

		if ((_1stHalf==0) && (_2ndHalf==1))// && (GetDaliRxErrorFlag()==0))			// Start bit received
			{
				DisableInt1 ();
				StopDaliRxTimer();

				//It will reload a one period worth of time, so it will start sampling on the next bit
				ReloadDaliRxTimer(TMH, TML);
				EnableDaliRxTimerInt();
				StartDaliRxTimer();
				State = DATA;

			}
		intcounter=0;
		_1stHalf = _2ndHalf =0;
	}
#endif


}



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

void ReloadnStartDaliRxTimer(uint8_t reloadH, uint8_t reloadL)
{
	TH1 = reloadH;
	TL1 = reloadL;
	EnableDaliRxTimerInt();
	StartDaliRxTimer();
}

void StopnDisableDaliRxTimer()
{

	StopDaliRxTimer();
	DisableDaliRxTimerInt();
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

void IsDaliBusQuiet()
{

	if (GetDaliIntputPin()==1)
	{

		if(GetBusQuietCounter()<=254) IncBusQuietCounter();
	}
	if (GetDaliIntputPin()==0)
	{

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

