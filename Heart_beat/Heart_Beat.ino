/**
 * This is the script for pinging the web server over the TCP/IP and UDP/IP
 * channels 
 * server is address;  port  
 * THIS SCRIPT WAS TESTED AND WORKS 
 * Date:12/08/2012
 **/
#include <SoftwareSerial.h>  //Include the NewSoftSerial library to send serial commands to the cellular module.
#include <string.h>         //Used for string manipulations
#include <AESLib.h>        // AES Encryption library

SoftwareSerial cell(2,3);  //Create a 'fake' serial port. Pin 2 is the Rx pin, pin 3 is the Tx pin.
char incoming_char;
void setup()
{
  //Initialize serial ports for communication.
  Serial.begin(9600);
  cell.begin(9600);
  Serial.println("Starting SM5100B-D communications");
}

void loop()
{
    if(cell.available() > 0)
    {
      incoming_char = cell.read();    //Get the character from the cellular serial port.
      Serial.print(incoming_char);
    }
    //If a character is coming from the terminal to the Arduino...
    if(Serial.available() > 0)
    {
      incoming_char = Serial.read();  //Get the character coming from the terminal
      cell.print(incoming_char);    //Send the character to the cellular module.
    }
}
