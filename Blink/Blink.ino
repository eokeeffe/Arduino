/*
  Blink
 Turns on an LED on for one second, then off for one second, repeatedly.
 
 This example code is in the public domain.
 */

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int leds[] = {8,10,12};

// the setup routine runs once when you press reset:
void setup() 
{                
  // initialize the digital pin as an output.
  int i;
  for(i=0;i<3;i++)
  {
    pinMode(leds[i], OUTPUT); 
  }    
}

// the loop routine runs over and over again forever:
void loop() 
{
  int i;
  for(i=0;i<3;i++)
  {
    digitalWrite(leds[i], HIGH);
    delay(4000);
    digitalWrite(leds[i], LOW);
    delay(2000);
  }
  
}

