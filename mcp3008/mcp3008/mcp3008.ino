
// http://playground.arduino.cc/Code/MCP3208
// http://arduino.cc/forum/index.php?topic=53082.0
// http://learn.adafruit.com/reading-a-analog-in-and-controlling-audio-volume-with-the-raspberry-pi/script

/*
Pinout:

    1-8 - chan 0-7 -> the 8 levels to be measured
    9 DGND -> GND
    10 CS chip select -> D10
    11 Din MOSI -> D11
    12 Dout MISO -> D12
    13 CLC clock -> D13
    14 AGN -> GND
    15 Vref -> reference voltage (that gives max adc reading)
    16 Vdd -> supply voltage, max 5.5V so Arduino 5V is fine 

        ___ 
CH0  1 | u | 16  VDD
CH1  2 |   | 15  VREF
CH2  3 |   | 14  AGND
CH3  4 |   | 13  CLK     --> D13
CH4  5 |   | 12  DOUT    --> D12
CH5  6 |   | 11  DIN     --> D11
CH6  7 |   | 10  CS/SHDN --> D10
CH7  8 |___| 9   DGND  


*/


#include <SPI.h>

int cs_pin = 10;


void setup()
{
  Serial.begin(9600);
  pinMode(cs_pin, OUTPUT);
  digitalWrite(cs_pin, HIGH);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);

  float time = micros();
  for(int i = 0;i<1000;i++)
  {
    readADC(0);
  }
  float time1 = micros();
  
  Serial.println((time1-time)/1000);
}

void loop()
{
  int reading = readADC(0);
  Serial.println(reading);
  delay(1000);
}


uint16_t readADC(int channel)
{
  uint16_t output;
  //Channel must be from 0 to 7
  //Shift bits to match datasheet for MCP3208
  byte commandMSB = B00000110;

  uint16_t commandBytes = (uint16_t) (commandMSB<<8|channel<<6);
  
  //Select ADC
  digitalWrite(cs_pin, LOW);
  //send start bit and bit to specify single or differential mode (single mode chosen here)
  SPI.transfer((commandBytes>>8) & 0xff);

  //byte msb = SPI.transfer((byte)commandBytes & 0xff) & B00001111;
  byte msb = SPI.transfer((byte)commandBytes & 0xff) & B00111111;  
  byte lsb = SPI.transfer(0x00);
  
  digitalWrite(cs_pin, HIGH);

  // cast before shiting the byte
  return ((uint16_t) msb) <<6 | lsb;
}
