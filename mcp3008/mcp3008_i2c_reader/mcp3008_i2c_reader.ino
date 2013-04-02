// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
int adc[8];


void setup()
{
  Serial.begin(9600);
  Serial.println("master_writer.ino [i2c #8]");
  Wire.begin(8); // join i2c bus (address optional for master)
  Wire.onReceive(receiveEvent); // register event
}

byte x = 0;

void loop()
{
  Wire.beginTransmission(9); // transmit to device #4
  //Wire.write("x is ");        // sends five bytes
  Wire.write(x);              // sends one byte  
  Wire.endTransmission();    // stop transmitting

  x++;
  if (x > 7) { x = 0; }
  delay(1500);
}


void receiveEvent(int howMany) {
  //Serial.println("receiveEvent");
  int reading = -1;
  //while(1 < Wire.available()) // loop through all but the last
  //{
  //  char c = Wire.read(); // receive byte as a character
  //  Serial.print(c);         // print the character
  //}
  int x = 0;
  while(Wire.available()){
    int val = Wire.read();
    adc[x] = val;
    Serial.print(adc[x]);
    Serial.print(" ");
    x++;
  }
  Serial.println();
  //int x = Wire.read();    // receive byte as an integer
  //Serial.println(x);         // print the integer

  
}
