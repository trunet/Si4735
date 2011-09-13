/* Arduino Si4735 Library
 * Written by Ryan Owens for SparkFun Electronics
 * 5/17/11
 *
 * This library is for use with the SparkFun Si4735 Shield
 * Released under the 'Buy Me a Beer' license
 * (If we ever meet, you buy me a beer)
 *
 * See the header file for better function documentation.
 *
 * See the example sketches to learn how to use the library in your code.
*/

#include "Si4735.h"
#include "WProgram.h"
#include "string.h"

//This is just a constructor.
Si4735::Si4735(){
	//Do Nothing
}


void Si4735::begin(char mode){
	_mode = mode;
	//Set the initial volume level.
	_currentVolume=63;

	//Start by resetting the Si4735 and configuring the comm. protocol to SPI
	pinMode(POWER_PIN, OUTPUT);
	pinMode(RADIO_RESET_PIN, OUTPUT);
	pinMode(DATAIN, OUTPUT);  //Data In (GPO1) must be driven high after reset to select SPI
	pinMode(INT_PIN, OUTPUT);  //Int_Pin (GPO2) must be driven high after reset to select SPI

	//Sequence the power to the Si4735
	digitalWrite(RADIO_RESET_PIN, LOW);  
	digitalWrite(POWER_PIN, LOW);

	//Configure the device for SPI communication
	digitalWrite(DATAIN, HIGH);
	digitalWrite(INT_PIN, HIGH);
	delay(1);
	digitalWrite(POWER_PIN, HIGH);
	delay(100);
	digitalWrite(RADIO_RESET_PIN, HIGH);
	delay(100);

	//Now configure the I/O pins properly
	pinMode(DATAOUT, OUTPUT);
	pinMode(DATAIN, INPUT);
	pinMode(SPICLOCK,OUTPUT);
	pinMode(SS,OUTPUT); 
	pinMode(INT_PIN, INPUT); 
	digitalWrite(SS, HIGH);	

	//Configure the SPI hardware
	SPCR = (1<<SPE)|(1<<MSTR);	//Enable SPI HW, Master Mode	
	
	//Send the POWER_UP command
	switch(_mode){
		case FM:
			sprintf(command, "%c%c%c", 0x01, 0x50, 0x05);
			break;
		case AM:
		case SW:
		case LW:
			sprintf(command, "%c%c%c", 0x01, 0x51, 0x05);
			break;
		default:
			return;
	}
	sendCommand(command, 3);
	delay(200);

	//Set the volume to the current value.
	sprintf(command, "%c%c%c%c%c%c", 0x12, 0x00, 0x40, 0x00, 0x00, _currentVolume);
	sendCommand(command, 6);
	delay(10);

	//Disable Mute
	sprintf(command, "%c%c%c%c%c%c", 0x12, 0x00, 0x40, 0x01, 0x00, 0x00);
	sendCommand(command, 6);
	delay(1);

	//Set the seek band for the desired mode (AM and FM can use default values)
	switch(_mode){
		case SW:
			//Set the lower band limit for Short Wave Radio to 2300 kHz
			sprintf(command, "%c%c%c%c%c%c", 0x12, 0x00, 0x34, 0x00, 0x08, 0xFC);
			sendCommand(command, 6);
			delay(1);
			//Set the upper band limit for Short Wave Radio to 23000kHz
			sprintf(command, "%c%c%c%c%c%c", 0x12, 0x00, 0x34, 0x01, 0x59, 0xD8);
			sendCommand(command, 6);			
			delay(1);
			break;
		case LW:
			//Set the lower band limit for Long Wave Radio to 152 kHz
			sprintf(command, "%c%c%c%c%c%c", 0x12, 0x00, 0x34, 0x00, 0x00, 0x99);
			sendCommand(command, 6);
			//Set the upper band limit for Long Wave Radio to 279 kHz
			sprintf(command, "%c%c%c%c%c%c", 0x12, 0x00, 0x34, 0x01, 0x01, 0x17);
			sendCommand(command, 6);			
			break;
		default:
			break;
	}
	
}

void Si4735::sendCommand(char * myCommand){
	char tempValue=0;
	int index=0;
	//Convert the ascii string to a binary string
	while(*myCommand != '\0'){
		if(toupper(*myCommand) > '9')tempValue = toupper(*myCommand)-'A'+10;
		else tempValue = *myCommand - '0';
		command[index] = tempValue * 16;
		*myCommand++;
		
		if(toupper(*myCommand) > '9')tempValue = toupper(*myCommand)-'A'+10;
		else tempValue = *myCommand - '0';
		command[index++] += tempValue;
		*myCommand++;
	}
	//Now send the command to the radio
	sendCommand(command, index);
}

/*
* Description: Tunes the Si4735 to a frequency
*
* Params: int frequency - The desired frequency in kHz
*
* Returns: True if tune was successful
*			False if tune was unsuccessful
*/
bool Si4735::tuneFrequency(int frequency){
	//Split the desired frequency into two character for use in the
	//set frequency command.
	char highByte = frequency >> 8;
	char lowByte = frequency & 0x00FF;
	
	//Depending on the current mode, set the new frequency.
	switch(_mode){
		case FM:
			sprintf(command, "%c%c%c%c", 0x20, 0x00, highByte, lowByte);
			break;
		case AM:
		case SW:
		case LW:
			sprintf(command, "%c%c%c%c", 0x40, 0x00, highByte, lowByte);
			break;
		default:
			break;
	}
	sendCommand(command, 4);
	delay(100);
	
	return true;
}

//This function does not work yet!
int Si4735::getFrequency(void){
	return 0;
}

bool Si4735::seekUp(void){
	//Use the current mode selection to seek up.
	switch(_mode){
		case FM:
			sprintf(command, "%c%c", 0x21, 0x0C);
			sendCommand(command, 2);
			break;
		case AM:
		case SW:
		case LW:
			sprintf(command, "%c%c%c%c%c%c", 0x41, 0x0C, 0x00, 0x00, 0x00, 0x00);
			sendCommand(command, 6);
			break;
		default:
			break;
	}
	delay(1);
	return true;
}

bool Si4735::seekDown(void){
	//Use the current mode selection to seek down.
	switch(_mode){
		case FM:
			sprintf(command, "%c%c", 0x21, 0x04);
			sendCommand(command, 2);
			break;
		case AM:
		case SW:
		case LW:
			sprintf(command, "%c%c%c%c%c%c", 0x41, 0x04, 0x00, 0x00, 0x00, 0x00);
			sendCommand(command, 6);
			break;
		default:
			break;
	}
	delay(1);
	return true;
}

void Si4735::volumeUp(void){
	//If we're not at the maximum volume yet, increase the volume
	if(_currentVolume < 63){
		_currentVolume+=1;
		//Set the volume to the current value.
		sprintf(command, "%c%c%c%c%c%c", 0x12, 0x00, 0x40, 0x00, 0x00, _currentVolume);
		sendCommand(command, 6);
		delay(10);		
	}
}

void Si4735::volumeDown(void){
	//If we're not at the maximum volume yet, increase the volume
	if(_currentVolume > 0){
		_currentVolume-=1;
		//Set the volume to the current value.
		sprintf(command, "%c%c%c%c%c%c", 0x12, 0x00, 0x40, 0x00, 0x00, _currentVolume);
		sendCommand(command, 6);
		delay(10);		
	}
}

void Si4735::mute(void){
	//Disable Mute
	sprintf(command, "%c%c%c%c%c%c", 0x12, 0x00, 0x40, 0x01, 0x00, 0x03);
	sendCommand(command, 6);
	delay(1);
}

void Si4735::unmute(void){
	//Disable Mute
	sprintf(command, "%c%c%c%c%c%c", 0x12, 0x00, 0x40, 0x01, 0x00, 0x00);
	sendCommand(command, 6);
	delay(1);
}

char Si4735::getStatus(void){
	char response;
	digitalWrite(SS, LOW);
	delay(1);
	spiTransfer(0xA0);  //Set up to read a single byte
	response = spiTransfer(0x00);  //Get the commands response
	digitalWrite(SS, HIGH);
	return response;
}
void Si4735::getResponse(char * response){
	digitalWrite(SS, LOW);
	delay(1);
	spiTransfer(0xE0);  //Set up to read the long response
	for(int i=0; i<16; i++)*response++ = spiTransfer(0x00);  //Assign the response to the string.
	digitalWrite(SS, HIGH);
}

void Si4735::end(void){
	sprintf(command, "%c", 0x11);
	sendCommand(command, 1);
	delay(1);
}

/*******************************************
*
* Private Functions
*
*******************************************/
char Si4735::spiTransfer(char value){
	SPDR = value;                    // Start the transmission
	while (!(SPSR & (1<<SPIF)))     // Wait for the end of the transmission
	{
	};
	return SPDR;                    // return the received byte
}

void Si4735::sendCommand(char * command, int length){
  digitalWrite(SS, LOW);
  delay(1);
  spiTransfer(0x48);  //Contrl byte to write an SPI command (now send 8 bytes)
  for(int i=0; i<length; i++)spiTransfer(command[i]);
  for(int i=length; i<8; i++)spiTransfer(0x00);  //Fill the rest of the command arguments with 0
  digitalWrite(SS, HIGH);  //End the sequence
}

