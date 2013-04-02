
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
#include <Wire.h>

int cs_pin = 10;
int value = 0;
int adc[8];
boolean sendFlag = false;

void setup()
{
  Serial.begin(9600);
  Serial.println("mcp3008_i2c.ino [i2c #9]");
  Wire.begin(9);                // Start I2C Bus as a Slave (Device Number 9)
  Wire.onReceive(receiveEvent); // register event
  pinMode(cs_pin, OUTPUT);
  digitalWrite(cs_pin, HIGH);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);
}


void loop()
{
  //int reading = readADC(0);
  //if (value > 0){
  //  sendValue();
  //}
  if (sendFlag){
    sendValues();
    sendFlag = false;
  }
  //Serial.println(reading);
  //delay(1000);
}


void readADCs(){
  for(int x=0; x < 8; x++){
    adc[x] = readADC(x);
    delay(10);
  }
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
  return ((uint16_t) msb) <<4 | lsb;
}


void receiveEvent(int howMany) {
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer

  if (x >= 0 || x < 8){
    //value = readADC(x);
    readADCs();
    sendFlag = true;
  };

}


void sendValues(){
  Wire.beginTransmission(8); // transmit to device #8
  for (int x = 0; x < 8; x++){
    Wire.write(adc[x]); 
    Serial.print(adc[x]);
    Serial.print(" ");
  }
  Serial.println();
  Wire.endTransmission();    // stop transmitting
}


void sendValue(){

  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(value);              // sends x 
  Wire.endTransmission();    // stop transmitting  
  value = 0;
  
}
