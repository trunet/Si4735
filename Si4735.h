/* Arduino Si4735 Library
 * Written by Ryan Owens for SparkFun Electronics
 * 5/17/11
 * Altered by Wagner Sartori Junior <wsartori@gmail.com> on 09/13/11
 *
 * This library is for use with the SparkFun Si4735 Shield
 * Released under the 'Buy Me a Beer' license
 * (If we ever meet, you buy me a beer)
 *
 * See the example sketches to learn how to use the library in your code.
*/

#ifndef Si4735_h
#define Si4735_h

#include "WProgram.h"
#include "SPI.h"

//Assign the radio pin numbers
#define POWER_PIN	8
#define	RADIO_RESET_PIN	9
#define INT_PIN	2

//Define the SPI Pin Numbers
#define DATAOUT 11		//MOSI
#define DATAIN  12		//MISO 
#define SPICLOCK  13	//sck
#define SS 10	        //ss

//List of possible modes for the Si4735 Radio
#define AM	0
#define	FM	1
#define SW	2
#define	LW	3

#define ON	true
#define OFF	false

#define MAKEINT(msb, lsb) (((msb) << 8) | (lsb))

class Si4735 : public SPIClass
{
	public:
		//This is just a constructor.
		Si4735();
		/*
		* Description: 
		*	Initializes the Si4735, powers up the radio in the desired mode and limits the bandwidth appropriately.
		* 	This function must be called before any other radio command.
		*	The bands are set as follows:
		*	FM - 87.5 - 107.9 MHz
		*	AM - 520 - 1710 kHz
		*	SW - 2300 - 23000 khz
		*	LW - 152 - 279 kHz
		* Parameters:
		*	mode - The desired radio mode. Use AM(0), FM(1), SW(2) or LW(3).
		*/
		void begin(char mode);
		/*
		* Description: 
		*	Used to send an ascii command string to the radio.
		* Parameters:
		*	myCommand - A null terminated ascii string limited to hexidecimal characters
		*				to be sent to the radio module. Instructions for building commands can be found
		*				in the Si4735 Programmers Guide.
		*/
		void sendCommand(char * myCommand);
		/*
		* Description: 
		*	Used to to tune the radio to a desired frequency. The library uses the mode indicated in the
		* 	begin() function to determine how to set the frequency.
		* Parameters:
		*	frequency - The frequency to tune to, in kHz (or in 10kHz if using FM mode).
		* Returns:
		*	True
		* TODO:
		* 	Make the function return true if the tune was successful, else return false.
		*/
		bool tuneFrequency(int frequency);
		/*
		* Description:
		*	This function currently does not work!
		* TODO:
		*	Make this function work.
		*/
		int getFrequency(bool &valid);
		/*
		* Description:
		*	Commands the radio to seek up to the next valid channel. If the top of the band is reached, the seek
		*	will continue from the bottom of the band.
		* Returns:
		*	True
		* TODO:
		*	Make the function return true if a valid channel was found, else return false.
		*/
		bool seekUp(void);
		/*
		* Description:
		*	Commands the radio to seek down to the next valid channel. If the bottom of the band is reached, the seek
		*	will continue from the top of the band.
		* Returns:
		*	True
		* TODO:
		*	Make the function return true if a valid channel was found, else return false.
		*/		
		bool seekDown(void);
		
		void readRDS(void);
		void getRDS(char * ps, char * radiotext);
		/*
		* Description:
		*	Increasese the volume by 1. If the maximum volume has been reached, no increase will take place.
		*/
		void volumeUp(void);
		/*
		* Description:
		*	Decreases the volume by 1. If the minimum volume has been reached, no decrease will take place.
		*/
		void volumeDown(void);
		
		void setVolume(byte volume);
		byte getVolume();
		/*
		* Description:
		*	Mutes the audio output
		*/
		void mute(void);
		/*
		* Description:
		*	Disables the mute.
		*/
		void unmute(void);
		/*
		* Description:
		*	Gets the current status of the radio. Learn more about the status in the Si4735 datasheet.
		* Returns:
		*	The status of the radio.
		*/
		char getStatus(void);
		/*
		* Description:
		*	Gets the long response (16 characters) from the radio. Learn more about the long response in the Si4735 datasheet.
		* Parameters:
		*	response - A string for the response from the radio to be stored in.
		*/
		void getResponse(char * response);
		/*
		* Description:
		*	Powers down the radio
		*/
		void end(void);
		
	private:
		char _disp[65]; // Radio Text
		char _ps[9]; // Program service name
		bool _ab; // Detect new radiotext
		/*
		* A variable that is assigned the current mode of the radio (AM, FM, SW or LW)
		*/
		char _mode;
		/*
		* A variable the keeps the current volume level. 
		*/
		char _currentVolume;
		/*
		* Command string that holds the binary command string to be sent to the Si4735.
		*/
		char command[9];
		/*
		* Description:
		*	Sends a binary command string to the Si4735.
		* Parameters:
		*	command - Binary command to be sent to the radio.
		*	length - The number of characters in the command string (since it can't be null terminated!)
		* TODO:
		*	Make the command wait for a valid CTS response from the radio before releasing control of the CPU.
		*/
		void sendCommand(char * command, int length);
		/*
		* Description:
		*	Sends/Receives a character from the SPI bus.
		* Parameters:
		*	value - The character to be sent to the SPI bus.
		* Returns:
		*	The character read from the SPI bus during the transfer.
		*/
		char spiTransfer(char value);
		
		void clearRDS(void);
};

#endif
