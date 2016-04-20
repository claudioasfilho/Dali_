//=========================================================
// inc/InitDevice.h: generated by Hardware Configurator
//
// This file will be regenerated when saving a document.
// leave the sections inside the "$[...]" comment tags alone
// or they will be overwritten!
//=========================================================
#ifndef __DALI_H__
#define __DALI_H__

#define TMH	0xff
#define TML	0x2B

#define STMH TMH
#define STML 0x95
//-----------------------------------------------------------------------------
#include <SI_EFM8UB1_Register_Enums.h>
//#include "InitDevice.h"

typedef union bits_byte
				{
					struct{
						uint8_t BB0:1;
						uint8_t BB1:1;
						uint8_t BB2:1;
						uint8_t BB3:1;
						uint8_t BB4:1;
						uint8_t BB5:1;
						uint8_t BB6:1;
						uint8_t BB7:1;
						} nybble;
					uint8_t Abyte;
					} BITS_BYTE;


typedef union daliflags
				{
					struct{
								uint8_t Busy:1;
								uint8_t Output:1;
								uint8_t Stop:1;
								uint8_t Error:1;
								uint8_t NU1:1;
								uint8_t NU2:1;
								uint8_t NU3:1;
								uint8_t NU4:1;
							} flag;
								uint8_t DRegister;
						} DALIFLAGS;

typedef enum
			{
				IDLE=0,
				START,
				ADDRESS,
				DATA,
				STOP,
				END
				} DALI_FRAME;

typedef enum
			{
				_1qB=0,	//First Quarter of the Bit
				_2qB,
				_3qB,
				_4qB
			} DALI_DEMOD;


typedef enum
{
	ACTIVE_LOW=0,
	ACTIVE_HIGH=1
}INTPOLARITY;

#define DALI_LOGIC_1 1
#define DALI_LOGIC_0 0

//-----------------------------------------------------------------------------
// Pin Definitions
//-----------------------------------------------------------------------------
//SI_SBIT (DISP_EN, SFR_P2, 3);          // Display Enable

SI_SBIT (PB0_SW,SFR_P0, 2);			   //PB0 Switch Definition
SI_SBIT (DALI_OUT, SFR_P0, 0);		   //Dali Output Pin
SI_SBIT (DALI_IN, SFR_P0, 7);		   //Dali Input Pin
SI_SBIT (LED2,SFR_P1, 4);			   //Test LED

#define ToogleTestLed() LED2^=1;

/* Manchester Encoder Methods */

void ManchesterEncoder (uint8_t input);
void SetBusyFlag();
void ClearBusyFlag();
bit GetBusyFlag();
bit GetMDOutput();
void SetMDOutput();
void ClearMDOutput();

void EnableDaliRxTimerInt();
void DisableDaliRxTimerInt();
void ReloadDaliTxTimer(uint8_t reloadH, uint8_t reloadL);
void StartDaliTxTimer();
uint16_t GetDaliTxTimer();
void StopDaliTxTimer();

void DaliFrameStart();
void DaliFrameStop();
void DaliTXStateMachine(uint8_t address, uint8_t Ddata);

void SetDaliOutputPin();
void ClearDaliOutputPin();
bit GetDaliOutputPin();
void DaliTxHandler();

void SetDaliStopFlag();
void ClearDaliStopFlag();
bit GetDaliStopFlag();


//Dali RX

void DaliRXDecoding();

void ReloadDaliRxTimer(uint8_t reloadH, uint8_t reloadL);
void StartDaliRxTimer();
void StopDaliRxTimer();
uint16_t GetDaliRxTimer();
void ReloadnStartDaliRxTimer(uint8_t reloadH, uint8_t reloadL);
void StopnDisableDaliRxTimer();

bit GetDaliIntputPin();
void IsDaliBusQuiet();
void IncBusQuietCounter();
void ClearBusQuietCounter();
uint8_t GetBusQuietCounter();

void SetDaliInputPinPolarity (INTPOLARITY input);
void EnableInt1 ();
void DisableInt1 ();



#endif

