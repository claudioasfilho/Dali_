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

static xdata DALIFLAGS DaliFlags;
static xdata DALIRXREGISTERS DaliRXReg;

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

void SetMDOutput()			//This function is processed by the ManchesterTXHandler and sets the DALI Output to Logic 1
{							//It is not setting the pin, but the logical state
	DaliFlags.flag.Output =1;
}

void ClearMDOutput()		//This function is processed by the ManchesterTXHandler and sets the DALI Output to Logic 0
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

void ManchesterTXHandler()
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

void DaliTXFrame(uint8_t address, uint8_t Ddata)
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

void DaliAnswerFrame(uint8_t Ddata)
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

void ClearDaliFlags()
{
	DaliFlags.DRegister=0;
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

static xdata DALI_FRAME ErrorLog = IDLE;


DALI_FRAME startconditionbitDemodulation()
{
	static xdata DALI_DEMOD bitState = _1qB;


	switch (bitState)
	{
		case _1qB: //This is Triggered by the INT1_ISR
		{
			_1stQ = GetDaliIntputPin();
			if((GetDaliIntputPin()==DALI_LOGIC_0))
			{

				ReloadnStartDaliRxTimer(STMH, STML);
				bitState = _2qB;
			}
			else
			{
				//Something is wrong with the Data, it Returns an Error
				bitState = _1qB;
				return ERRORRESET;
			}
			break;
		}

		case _2qB: //This is Triggered by the TIMER1_ISR
		{
			_2ndQ = GetDaliIntputPin();
			if((GetDaliIntputPin()==DALI_LOGIC_0))
			{

				StopnDisableDaliRxTimer();
				SetDaliInputPinPolarity(ACTIVE_HIGH);
				EnableInt1();
				bitState = _3qB;

			}
			else
			{
				//Something is wrong with the Data, it Returns an Error
				bitState = _1qB;
				return ERRORRESET;
			}
			break;
		}

		case _3qB: //This is Triggered by the TIMER1_ISR
		{
			_3rdQ = GetDaliIntputPin();
			if((GetDaliIntputPin()==DALI_LOGIC_1))
			{


				ReloadnStartDaliRxTimer(STMH, STML);
				bitState = _4qB;


			}
			else
			{
				//Something is wrong with the Data, it Returns an Error
				bitState = _1qB;
				return ERRORRESET;
			}
			break;

		}
		case _4qB: //This is Triggered by the TIMER1_ISR, once completed successfully it disables the INT1 and it uses the Timer to check the DALI_IN State
		{
			_4thQ = GetDaliIntputPin();
			if((GetDaliIntputPin()==DALI_LOGIC_1))
			{

				DisableInt1();
				ReloadnStartDaliRxTimer(STMH, STML);
				bitState = _1qB;

				return ADDRESS; //Processing is done. It resets this state machine and moves the main state machine to the next state
			}
			else
			{
				//Something is wrong with the Data, it resets the INT trigger and Disables Timer
				bitState = _1qB;
				return ERRORRESET;
			}
			break;
		}

	}

	return START; //Still Processing the Bit
}

DALI_FRAME stopconditionbitverify()
{
	static xdata DALI_DEMOD bitState = _1qB;


	switch (bitState)
	{
		case _1qB: //This is Triggered by the TIMER1_ISR
		{
			if((GetDaliIntputPin()==DALI_LOGIC_1))
			{

				_1stQ = GetDaliIntputPin();
				ReloadDaliRxTimer(TMH, TML);
				bitState = _2qB;
			}
			else //If the bit is corrupted, the device goes to ERRORRESET MODE
			{
				bitState = _1qB;
				return ERRORRESET;
			}
			break;
		}

		case _2qB: //This is Triggered by the TIMER1_ISR
		{
			if((GetDaliIntputPin()==DALI_LOGIC_1))
			{

				_2ndQ = GetDaliIntputPin();
				ReloadDaliRxTimer(TMH, TML);
				bitState = _3qB;
			}
			else //If the bit is corrupted, the device goes to ERRORRESET MODE
			{
				bitState = _1qB;
				return ERRORRESET;
			}
			break;
		}

		case _3qB: //This is Triggered by the TIMER1_ISR
		{
			if((GetDaliIntputPin()==DALI_LOGIC_1))
			{

				_3rdQ = GetDaliIntputPin();
				ReloadDaliRxTimer(TMH, TML);
				bitState = _4qB;

			}
			else //If the bit is corrupted, the device goes to ERRORRESET MODE
			{
				bitState = _1qB;
				return ERRORRESET;
			}
			break;

		}
		case _4qB: //This is Triggered by the TIMER1_ISR, once completed successfully it disables the INT1 and it uses the Timer to check the DALI_IN State
		{
			if((GetDaliIntputPin()==DALI_LOGIC_1))
			{

				_4thQ = GetDaliIntputPin();
				bitState = _1qB;
				return END; //Processing is done. It resets this state machine and moves the main state machine to the next state
			}
			else //If the bit is corrupted, the device goes to ERRORRESET MODE
				{
					bitState = _1qB;
					return ERRORRESET;
				}
			break;
		}

	}

	return STOP; //Still Processing the Bit
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
				//ToogleTestLed1();
				break;
			}

			case _2qB: //This is Triggered by the TIMER1_ISR
			{
				_2ndQ = GetDaliIntputPin();
				ReloadnStartDaliRxTimer(STMH, STML);
				bitState = _3qB;
				//ToogleTestLed2();
				break;
			}

			case _3qB: //This is Triggered by the TIMER1_ISR
			{

				_3rdQ = GetDaliIntputPin();
				ReloadnStartDaliRxTimer(STMH, STML);
				bitState = _4qB;
				//ToogleTestLed3();
				break;

			}
			case _4qB: //This is Triggered by the TIMER1_ISR
			{

				_4thQ = GetDaliIntputPin();
				ReloadnStartDaliRxTimer(STMH, STML);
				bitState = _1qB;
				//ToogleTestLed4();
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

	//It Resets the "Global" bits (File Scope only) that are used to store the data read from the GPIO
	_1stQ=0;
	_2ndQ=0;
	_3rdQ=0;
	_4thQ=0;

	return output;
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

void SetDaliDataReadyFlag()
{
	DaliFlags.flag.Dataready = 1;
}

void ClearDaliDataReadyFlag()
{
	DaliFlags.flag.Dataready = 0;
}

bit GetDaliDataReadyFlag()
{
	return DaliFlags.flag.Dataready;
}

void SetDaliInputPinPolarity (INTPOLARITY input)
{
	if (input==ACTIVE_HIGH) IT01CF |= 0x80;
	else IT01CF &=0x7f;
}


void EnableInt1 ()
{
	TCON &= 0xF7; //Clearing the interrupt
	IE |= 0x4;
}

void DisableInt1 ()
{
	IE &= 0xfb;
}


void DaliRXDecoding(int EntryMethod, DALIMODE mode)
{
	static xdata DALI_FRAME State = IDLE;
	int Entry;

	static xdata BITS_BYTE DaliData;		//Variable used to temporarily store the bits being read from the Dali Command
	static xdata uint8_t bitscounter=7;		//Keeps the position of the current bit being read
	int bitread=0;							//Used to store the current bit being read. if the value is -1, it means the data was invalid

	Entry=EntryMethod;

	switch (State)

	   		{

				case IDLE:			//In this state, it checks if the RX bus was quite and also if it receives the start bit
		   					{
		   						if (GetBusQuietCounter()>1) State = START;
		   						DaliData.Abyte=0;
		   						DaliFlags.DRegister=0;		//It resets all the Flags
		   						DaliFlags.flag.RXBusy=1;		//It sets the Flag indicating the Dali Rx is busy

		   						//This Jumps straight to the next State on the State Machine so we don't loose a cycle, it purposely doesn't have a break
		   					}

				case START:			//In this state, it checks if the RX bus was quite and also if it receives the start bit
		   					{
		   						State = startconditionbitDemodulation();
		   						if (State==ERRORRESET) ErrorLog = START;
		   						break;
		   					}

	   			case ADDRESS:			//In this state, it gets the Data coming from the Dali Packet and load it to the Address Register and to the Data Register
	   			case DATA:
	   					{

	   						if (bitDemodulation())	//Is Demodulation done for the bit?
								{
									bitread = isbitHighorLow();

									if (bitread!=-1)
									{
										switch (bitscounter)
										{
											case 0: DaliData.nybble.BB0 = bitread;
													break;

											case 1: DaliData.nybble.BB1 = bitread;
													break;

											case 2: DaliData.nybble.BB2 = bitread;
													break;

											case 3: DaliData.nybble.BB3 = bitread;
													break;

											case 4: DaliData.nybble.BB4 = bitread;
													break;

											case 5: DaliData.nybble.BB5 = bitread;
													break;

											case 6: DaliData.nybble.BB6 = bitread;
													break;

											case 7: DaliData.nybble.BB7 = bitread;
													break;
										}
										if(bitscounter--==0)
										{

											bitscounter=7;
											if (State==ADDRESS)	//Reading Address
											{
												DaliRXReg.Address= DaliData.Abyte;

												if (mode==SLAVE_MODE)	State=DATA;
												else State=STOP;
											}
											else				//Reading Data
											{
												DaliRXReg.Data= DaliData.Abyte;
												State= STOP;
												ReloadnStartDaliRxTimer(TMH,TML); //Reloads the Timer for the STOP bit condition

											}

										}

									}

									else //If any of the bits is corrupted, the device goes to ERRORRESET MODE
									{
										ErrorLog = State;
										State = ERRORRESET;

									}
							}
	   						break;
	   					}
				case STOP:			//In this state, it checks if the RX bus was quite and also if it receives the start bit
		   					{
		   						State = stopconditionbitverify();
		   						if (State==ERRORRESET) ErrorLog = STOP;

		   						if (State==STOP)	break;
		   					}
				case END:			//In this state, it Resets the state machine and Turn on the Data Ready Flag
		   					{
		   						State = IDLE;
		   						ErrorLog = IDLE;
		   						StopnDisableDaliRxTimer();
		   						SetDaliInputPinPolarity(ACTIVE_LOW);
		   						EnableInt1();
		   						DaliFlags.flag.Dataready = 1;
		   						DaliFlags.flag.RXBusy=0;		//It Clears the Flag indicating the Dali Rx is not busy
		   						break;
		   					}

				case ERRORRESET:			//In this state, it Resets the state machine and Turn on the Data Ready Flag
		   					{
		   						State = IDLE;
		   						StopnDisableDaliRxTimer();
		   						SetDaliInputPinPolarity(ACTIVE_LOW);
		   						EnableInt1();

								DaliFlags.flag.Error = 1;
		   						break;
		   					}

	   			default: State=IDLE;



	   		}
}
