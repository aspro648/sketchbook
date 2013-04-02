// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup()
{
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  Serial.begin(9600);           // start serial for output
  Serial.println("i2c_slave_example");
  Serial.println("address = 4");
}

void loop()
{
  delay(100);
}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  char cmd;
  while(1 < Wire.available()) // loop through all but the last
  {
    cmd = Wire.read(); // receive byte as a character
    //Serial.println(cmd);         // print the character
  }
  int val = Wire.read();    // receive byte as an integer
  Serial.print("receiveEvent() ");
  Serial.print(cmd);
  Serial.print(" "); 
  Serial.println(val);         // print the integer
}


void requestEvent()
{
  byte val = 1;
  Serial.println("requestEvent() -> h");
  //Wire.beginTransmission();
  Wire.write(val); 

  //Wire.endTransmission();                     
}
