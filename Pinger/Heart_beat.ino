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
//----------------------------------------------------------------------------------------------------
//Example at+strsend ; cell.println("AT+SSTRSEND=1,\"THIS TEXT IS GETTING SEND!\"");,
char O2_Internet_Settings[] = "AT+CGDCONT=1,\"IP\",\"Internet\"\0"; //I tried a null at the end
char PDP_CONTEXT[] = "AT+CGPCO=0,\"\",\"\",1";
char PDP_ACTIVATE[] = "AT+CGACT=1,1";
char HOST_SETTINGS[] = "AT+SDATACONF=1,\"UDP\",\"ServerAddress\",ServerPORT";
char START_CONNECTION[] = "AT+SDATASTART=1,1";
char QUERY_SOCKET_STATUS[] = "AT+SDATASTATUS=1";
char* SEND_DATA = "AT+SSTRSEND=1,";
char UDP_STATUS[] = "AT+SDATASTATUS=1";
char CLOSE_CONNECTION[] = "AT+SDATASTART=1,0";
//----------------------------------------------------------------------------------------------------
// these are the at response strings
char sim_set[] = "+SIND: 10";
char found_net[] = "+SIND: 11";
char partial_ready[] = "+SIND: 3"; 
char ready[] = "+SIND: 4";
char WARNING1[] = "+SIND: 7";
char WARNING2[] = "+SIND: 8";
//----------------------------------------------------------------------------------------------------
// Key for encrypting the message and the message
uint8_t key[] = {
  1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
char* data = "\"Almost Done with the program now\"";// String to identify the customer to the server
//----------------------------------------------------------------------------------------------------
// String for incoming messages
char incoming_char;
int const BUFFER_SIZE = 180;
char AT_INCOMING[180];
int i=0;
int counter=0;
//----------------------------------------------------------------------------------------------------
// Booleans for turning on certain sequences of the program
boolean pinEntered=false;
boolean start_up[4];
boolean all_ready=false;
boolean network_connected = false;
boolean message_sent = false;
boolean unlock_string = false;
//----------------------------------------------------------------------------------------------------
// These are status LED pins
int Message_Sent = 8;
int No_Signal = 10;
int Warning = 12;
//----------------------------------------------------------------------------------------------------
int reset_counter=0;

void setup()
{
  // initialize the digital pin as an output
  pinMode(Message_Sent, OUTPUT);
  pinMode(No_Signal, OUTPUT);
  pinMode(Warning, OUTPUT);

  //Initialize serial ports for communication.
  Serial.begin(9600);
  cell.begin(9600);
  // concatenate the strings for the at message
  SEND_DATA = strcat(SEND_DATA,data);

  for(i=0;i<4;i++)
  {
    start_up[i]=false;
  }
  i=0;
  clearATString();

  Serial.println("Starting SM5100B-D communications");
  // Delay the board for 10 seconds to wait for the module to boot up
  delay(10000);
}

void loop()
{
  if(pinEntered == false)//enter the pin code
  {
    Enter_Pin();
  }
  if(all_ready == true)//check the gprs signal and sent the message
  {
    check_GPRS();
  }
  if(network_connected == true)
  {
    HEART_BEAT();
  }
  else
  {
    //If a character comes in from the cellular module...
    if(cell.available() > 0)
    {
      incoming_char = cell.read();    //Get the character from the cellular serial port.
      //Serial.print(incoming_char);
      if(incoming_char == '\n')
      { 
        AT_INCOMING[i] = '\0';
        i++;

        if( strstr(AT_INCOMING,sim_set) )
        {
          start_up[0] = true;
          Serial.println("Found sim phonebooks");
        }
        else if( strstr(AT_INCOMING,found_net) )
        {
          start_up[1] = true;
          Serial.println("Found networks");
        }
        else if( strstr(AT_INCOMING,partial_ready) )
        {
          start_up[2] = true;
          Serial.println("Module partially ready");
        }
        else if( strstr(AT_INCOMING,ready) )
        {
          start_up[3] = true;
          Serial.println("Module fully ready");
        }
        else if( strstr(AT_INCOMING,WARNING1) )
        {
          digitalWrite(Warning, HIGH);
          digitalWrite(Message_Sent, LOW);
        }
        else if( strstr(AT_INCOMING,WARNING2) )
        {
          digitalWrite(Warning, HIGH);
          digitalWrite(Message_Sent, LOW);
        }
        else
        {
          Serial.println(AT_INCOMING);
        }
        clearATString();//clear AT_INCOMING and set i=0
        all_ready = start_up_sequence();
      }
      else
      {
        AT_INCOMING[i] = incoming_char;  // place the character into the buffer
        i++;
      }
    }
    //If a character is coming from the terminal to the Arduino...
    if(Serial.available() > 0)
    {
      incoming_char = Serial.read();  //Get the character coming from the terminal
      cell.print(incoming_char);    //Send the character to the cellular module.
    }
  }
}

void Enter_Pin()
{
  Serial.println("Entering Pin now");
  cell.println("AT+CPIN=0000");
  pinEntered = true;
  digitalWrite(Message_Sent, HIGH);
  digitalWrite(No_Signal, HIGH);
  digitalWrite(Warning, HIGH);
  delay(1000);
  digitalWrite(Message_Sent, LOW);
  //digitalWrite(No_Signal, LOW);
  digitalWrite(Warning, LOW);

  delay(10000);
}

void check_GPRS()//check to see if the GPRS is attached
{
  cell.println("AT+CGATT?");
  delay(4000);
  for(i=0 ; cell.available() > 0 ; i++)
  {
    AT_INCOMING[i] = cell.read();
  }
  Serial.println(AT_INCOMING);
  if(strstr(AT_INCOMING,"+CGATT: 1"))
  {
    Serial.println("GPRS attached,sending data");
    digitalWrite(No_Signal, LOW);
    digitalWrite(Warning, LOW);

    if(network_connected == false)//realistically only called once to set the data on start up
    {
      openConnection();
    }
  }
  else
  {
    reset_counter++;
    Serial.println("GPRS not Attached/detected");
    digitalWrite(Warning, HIGH);   // turn the LED on (HIGH is the voltage level)
  }

  if(reset_counter > 15)
  {
    reset_counter = 0;
    software_Reset(); 
  }

  clearATString();
  delay(2000);
}

boolean start_up_sequence()
{
  int a;
  boolean temp = true;
  // if all values are true then the network is set
  for(a = 0 ; a < 4 ; a++)
  {
    temp &= start_up[a];
  }
  if(temp == true)
  {
    digitalWrite(No_Signal,LOW);
  }
  return temp;
}

void clearATString()
{
  int a;
  for(a=0 ; a < BUFFER_SIZE ; a++)
  {
    AT_INCOMING[a]='\0';
  }
  i=0;
}

void HEART_BEAT()
{
  //encrpyt the message
  //aes128_enc_single(key,data);
  //data now holds the encrypted message

  Serial.print("Sending Data");
  cell.print('\r');
  cell.println(SEND_DATA);
  waitForOK();

  Serial.println("Checking how much data was sent");
  cell.print(UDP_STATUS);
  cell.print('\r');
  waitForOK();

  if(counter == 20)
  {
    Serial.println("Sent data exceeded, now clearing buffer");
    cell.print("AT+SDATASTATUS=0");
    cell.print('\r');
    waitForOK();
  }
  /*
  Serial.println(O2_Internet_Settings);
   Serial.println(PDP_CONTEXT);
   Serial.println(PDP_ACTIVATE);
   Serial.println(HOST_SETTINGS);
   Serial.println(START_CONNECTION);
   Serial.println(QUERY_SOCKET_STATUS);
   Serial.println(SEND_DATA);
   Serial.println(UDP_STATUS);
   Serial.println(CLOSE_CONNECTION);
   */

  counter++;
  message_sent = true;
  Serial.println("Message Sent ;-)");
  digitalWrite(Message_Sent, HIGH);   // turn the LED on (HIGH is the voltage level)
}

//reads through the buffer until it reaches the letter 'K'
boolean waitForOK()// returns false if true
{
  clearATString();
  delay(2000);
  while(cell.available() > 0)
  {
    incoming_char = cell.read();
    //Serial.print(incoming_char);
    AT_INCOMING[i] = incoming_char;
    i++;
  }
  if(strstr(AT_INCOMING,"OK"))
  {
    return false;
  }
  else
  {
    Serial.println(AT_INCOMING);
    return true;
  }
}

void openConnection()//open the connection to the server
{
  Serial.println("Setting up internet settings");
  cell.print(O2_Internet_Settings);
  cell.print('\r');
  waitForOK();

  Serial.println("Setting PDP Context");
  cell.print(PDP_CONTEXT);
  cell.print('\r');
  waitForOK();

  Serial.println("Activating PDP");
  cell.print(PDP_ACTIVATE);
  cell.print('\r');
  waitForOK();

  Serial.println("Entering host settings");
  cell.print(HOST_SETTINGS);
  cell.print('\r');
  waitForOK();

  Serial.println("Connecting to server");
  cell.print(START_CONNECTION);
  cell.print('\r');
  waitForOK();

  Serial.println("Checking connection");
  cell.print(QUERY_SOCKET_STATUS);
  cell.print('\r');
  waitForOK();

  network_connected = true;
}

void closeConnection()//close the connection to the server
{
  Serial.print("Closing connection");
  cell.print('\r');
  cell.println(CLOSE_CONNECTION);
  waitForOK(); 
}

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
  asm volatile ("jmp 0");
}   


