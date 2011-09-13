/*
* Si4735 Serial Example Sketch
* Written by Ryan Owens for SparkFun Electronics
*
* This example sketch allows a user to construct their own commands for the Si4735 and send them through the serial terminal.
* Learn more about the Si4735 commands in the Si4735 Programming Guide.
*
* HARDWARE SETUP:
* This sketch assumes you are using the Si4735 Shield from SparkFun Electronics. 
* The shield should be plugged into an Arduino Main Board (Uno, Duemillinove or similar).
*
* USING THE SKETCH:
* Once you've plugged the Si4735 Shield into your Arduino board, connect the Arduino to your computer
* and select the corresponding board and COM port from the Tools menu and upload the sketch. After
* the sketch has been updated, open the serial terminal using a 9600 bps baud speed. Using the Si4735 Programming Guide as a reference,
* you can use the serial terminal to send ascii commands to the Si4735. The command must be terminated with either a newline or carriage
* return character. To enable this in the Arduino Serial Terminal, make sure the drop down menu located in the bottom right hand side of the 
* terminal window (not the baud rate one, but right next to that) reads 'Newline,' 'Carriage Return,' or 'Both NL and CR.'
* The sketch will convert the ascii command to the corresponding binary command and send it to the radio. The command string may only consist
* of hexadecimal character (0-9, a-f, A-F). To get the current status of the radio, the letter 's' may be sent; likewise to get the latest 
* response from the radio the character 'r' may be sent. You can read more about the status and response strings in the Si4735 datasheet.
*
* SAMPLE COMMANDS
* 11 - Power down the radio
* 015005 - Power up the radio in FM mode. (INT pin enabled, external oscillator used, analog outputs enabled)
* 015105 - Power up the radio in AM mode. (Same configuration as FM) Note - This mode is used for AM, short wave and long wave modes.
* 20002602 - FM Tune command, sets frequency to 97.3 MHz (2602 Hex = 9730 Decimal)
* 40000352 - AM Tune command, sets frequency to 850 KHz (352 Hex = 850 Decimal)
* 1200340008FC - Set the lower band limit to 2300 (8FC Hex = 2300 Decimal)
* 1200340159D8 - Set the upper band limit to 23000
*
* NOTES:
* This sketch will start the Si4735 in FM mode. Other modes are AM, SW and LW. Check out the datasheet for more information on these
* modes. All of the functions in the library will work regardless of which mode is being used; however the user must indicate
* which mode is to be used in the begin() function. See the library documentation for more information.
*/
//Add the Si4735 Library to the sketch
#include <Si4735.h>

//Create an instance of the Si4735 named radio.
Si4735 radio;
//Create a string used for sending and receiving commands from the Si4735.
char interfaceString[17];
//Simple indexing variable used to keep track of the current location within the string
char commandIndex=0;
//Variable for holding the current character read from the Serial port.
char inChar=0;

void setup()
{
  //Create a serial connection
  Serial.begin(9600);

  //Initialize the radio
  radio.begin(FM);
  //Set the FM Frequency
  radio.tuneFrequency(9730);  
}

void loop()
{       
  //Wait to do something until a character is received on the serial port.
  if(Serial.available()>0){
    //Copy the incoming character to a variable.
    inChar = Serial.read();
    //Depending on the incoming character, decide what to do.
    switch(inChar){
      //If we get the letter 'r', print the response string from the radio to the terminal.
      case 'r':
      case 'R':
          //Clear whatever information is currently in the string.
          for(int i=0; i<17; i++)interfaceString[i]='\0';
          //Get the latest response from the radio.
          radio.getResponse(interfaceString);
          //Print all 16 characters in the response to the terminal.
          for(int i=0; i<16; i++){
            Serial.print((unsigned char)interfaceString[i], HEX);
            Serial.print(" ");
          }
          Serial.println();
          break;
      //If we get the letter 's', print the current status of the radio.
      case 's':
      case 'S':
          Serial.println((unsigned char)radio.getStatus(), HEX);
          break;
      //If we get a newline character or carriage return character, send the command to the radio.
      case 10:
      case 13:
          //Terminate the string with a null character
          interfaceString[commandIndex++]='\0';
          //Reset the index so it's ready for the next command.
          commandIndex=0;
          //Send the current command to the radio.
          radio.sendCommand(interfaceString);
        break;
      //If we get any other character, and it's a valid hexidecimal character, copy it to the command string.
      default: if((inChar >= '0' && inChar <= '9')||(inChar >= 'a' && inChar <= 'f')||(inChar >= 'A' && inChar <= 'F'))
                interfaceString[commandIndex++]=inChar;
        break;
    }
  }   
}

