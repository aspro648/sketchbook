#include "TinyWireS.h"           

#define I2C_SLAVE_ADDR  2    

void setup()
{
  // set Pins
  pinMode(5,OUTPUT);
  analogWrite(5,0);

  // init I2C Slave mode
  TinyWireS.begin(I2C_SLAVE_ADDR);
}

void loop()
{
  if (TinyWireS.available()){
    byte value = TinyWireS.receive();
    analogWrite(5, value);
  }
}
