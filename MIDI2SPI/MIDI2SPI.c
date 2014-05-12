////////////////////////////////////////////////////////////
//
// MIDI INPUT TO SPI
//
// Code for PIC12F1822
// Compiled with SourceBoost C
//
// hotchk155/2014
//
// Firmware version 
// 1.00 
//
////////////////////////////////////////////////////////////

#include <system.h>
#include <memory.h>

// 8MHz internal oscillator block, reset disabled
#pragma DATA _CONFIG1, _FOSC_INTOSC & _WDTE_OFF & _MCLRE_OFF &_CLKOUTEN_OFF
#pragma DATA _CONFIG2, _WRT_OFF & _PLLEN_OFF & _STVREN_ON & _BORV_19 & _LVP_OFF
#pragma CLOCK_FREQ 8000000
typedef unsigned char byte;

#define P_LED		porta.2

volatile byte xx=0;
////////////////////////////////////////////////////////////
// INTERRUPT HANDLER CALLED WHEN CHARACTER RECEIVED AT 
// SERIAL PORT
void interrupt( void )
{
	// check if this is serial rx interrupt
	if(pir1.5)
	{
		P_LED = !P_LED;

		// get the byte
		byte b = rcreg;
/*				
		switch(b)
		{
			case 0xf8:
				P_SYNCH = 1;
				bSynchCount = OUTPUT_HIGH_TIME;
				break;
			case 0xfa: // START
				P_RESTART = 1;
				P_RUN = 1;
				bRestartCount = OUTPUT_HIGH_TIME;
				break;
			case 0xfb: // CONTINUE
				P_RUN = 1;
				break;
			case 0xfc: // STOP
				P_RUN = 0;
				break;
		}				*/
	}
	
	// check if SPI slave interrupt
	if(pir1.3)
	{
		static const char *qq = "HELLO";
		if(xx++>4) xx=0;
		ssp1buf=(qq[xx]);		
		pir1.3=0;
	}
}

////////////////////////////////////////////////////////////
// INITIALISE SERIAL PORT FOR MIDI
void init_usart()
{
	pir1.1 = 1;		//TXIF 		
	pir1.5 = 0;		//RCIF
	
	pie1.1 = 0;		//TXIE 		no interrupts
	pie1.5 = 1;		//RCIE 		interrupt on receive
	
	baudcon.4 = 0;	// SCKP		synchronous bit polarity 
	baudcon.3 = 1;	// BRG16	enable 16 bit brg
	baudcon.1 = 0;	// WUE		wake up enable off
	baudcon.0 = 0;	// ABDEN	auto baud detect
		
	txsta.6 = 0;	// TX9		8 bit transmission
	txsta.5 = 0;	// TXEN		transmit enable
	txsta.4 = 0;	// SYNC		async mode
	txsta.3 = 0;	// SEDNB	break character
	txsta.2 = 0;	// BRGH		high baudrate 
	txsta.0 = 0;	// TX9D		bit 9

	rcsta.7 = 1;	// SPEN 	serial port enable
	rcsta.6 = 0;	// RX9 		8 bit operation
	rcsta.5 = 1;	// SREN 	enable receiver
	rcsta.4 = 1;	// CREN 	continuous receive enable
		
	spbrgh = 0;		// brg high byte
	spbrg = 15;		// brg low byte (31250)	
	
}

////////////////////////////////////////////////////////////
// ENTRY POINT
void main()
{ 
	//
	// CONFIGURE OSCILLATOR
	//
	// osc control / 8MHz / internal
	osccon = 0b01110010;
	
	//
	// CONFIGURE PIN FUNCTONS
	//	1	VDD-		   -VSS 9
	//	2	 RX-RA5		RA0-SS# 8
	//	3	SDO-RA4		RA1-SCL 7
	//	4  MCLR-RA3		RA2-LED	6
	//
	apfcon.7=1; // RX on RA5
	apfcon.6=1; // SDO on RA4
	apfcon.5=1; // SS# on RA0
	apfcon.2=1; // TX on RA4
	
	//
	// CONFIGURE IO
	//
	trisa.0=1; // SS# as input
	trisa.1=1; // SCK as input
	trisa.2=0; // SDI not used (used for LED, set as output)
	trisa.3=1; // MCLR#
	trisa.4=0; // SDO as output
	trisa.5=1; // UART RX as input
	ansela=0;
	porta=0;
	
	//
	// CONFIGURE SPI SLAVE
	//
	ssp1stat.7 = 0; // SMP must be cleared in SPI slave mode
	ssp1stat.6 = 0; // CKE clock edge (0=rising, 1=falling)
	
	ssp1con1.5 = 1; // SSP1EN enable spi port
	ssp1con1.4 = 0; // CLP clock polarity (0=idle low, 1=idle high)
	ssp1con1.3 = 0; // }
	ssp1con1.2 = 1; // } SSP1M mode 0100 = SPI slave with SS
	ssp1con1.1 = 0; // }
	ssp1con1.0 = 0; // }
	
	ssp1con3.4 = 0; // BOEN buffer overflow (0=halt, 1=continue)

	//
	// CONFIGURE INTERRUPTS
	// 
	intcon.7 = 1; // GIE
	intcon.6 = 1; // PEIE
	pie1.5 = 1;	  // RCIE
	pie1.3 = 1;	  // SSP1IE
	
	//
	// INIT USART INPUT
	//
	init_usart();

	// startup flash
	P_LED=1; delay_ms(200);
	P_LED=0; delay_ms(200);
	P_LED=1; delay_ms(200);
	P_LED=0; delay_ms(200);
	P_LED=1; delay_ms(200);
	P_LED=0; delay_ms(200);

	// loop forever		
	unsigned long count = 0;
	for(;;)
	{/*
		P_LED = ((count & 0xf000UL) == 0x1000UL);
		++count;
			
		if(bSynchCount)
		{
			if(!--bSynchCount) 
				P_SYNCH = 0;
		}
		if(bRestartCount)
		{
			if(!--bRestartCount) 
				P_RESTART = 0;
		}		*/
	}
}