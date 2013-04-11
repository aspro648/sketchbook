/* ATtiny85 as an I2C Slave            BroHogan                           1/12/11
 * Example of ATtiny I2C slave receiving and sending data to an Arduino master.
 * Gets data from master, adds 10 to it and sends it back.
 * SETUP:
 * ATtiny Pin 1 = (RESET) N/U                      ATtiny Pin 2 = (D3) N/U
 * ATtiny Pin 3 = (D4) to LED1                     ATtiny Pin 4 = GND
 * ATtiny Pin 5 = I2C SDA on DS1621  & GPIO        ATtiny Pin 6 = (D1) to LED2
 * ATtiny Pin 7 = I2C SCK on DS1621  & GPIO        ATtiny Pin 8 = VCC (2.7-5.5V)
 * NOTE! - It's very important to use pullups on the SDA & SCL lines!
 * Current Rx & Tx buffers set at 32 bytes - see usiTwiSlave.h
 * Credit and thanks to Don Blake for his usiTwiSlave code. 
 * More on TinyWireS usage - see TinyWireS.h
 */
// http://playground.arduino.cc/Code/USIi2c

/*

CONTROL REV 1

D0 - MTRA_1
D1 - gnd
D2 - gnd
D3 - MTRA_2
D4 -        SCL
D5 - ENA_A 
D6 -        SDA
D7 - ENA_B
D8 - MTRB_1
D9 - nc
D10- MTRB_2
 */

#include "TinyWireS.h"                  // wrapper class for I2C slave routines

#define I2C_SLAVE_ADDR  0x26            // i2c slave address (38)

int val;
char cmd;

int A = 0;
int B = 1;
int pins[] = {0, 1, 2, 3, 5, 7, 8, 10};
int MTR[2][2] = {
  {0, 3},
  {8, 10}
};
int ENA[2] = {5, 7};
int limit[2] = {100, 100};


void setup(){
  TinyWireS.begin(I2C_SLAVE_ADDR);      // init I2C Slave mode
  for(int pin = 0; pin < 8; pin++){
    pinMode(pins[pin], OUTPUT);
    digitalWrite(pins[pin], LOW);
  }   
}

/*
1 - mtr A fwd
2 - mtr A back
3 - mtr B fwd
4 - mtr B back
*/

void loop(){
  i2c_scan();
  if (val){
    if (val == 127){setup();}
    if (cmd > 0 && cmd < 5){ // legit mtr command
      if (cmd == 1){fwd(0, val);}
      if (cmd == 2){back(0, val);}
      if (cmd == 3){fwd(1, val);}
      if (cmd == 4){back(1, val);}      
    }
    TinyWireS.send(cmd);
    tws_delay(10);
    TinyWireS.send(val);   
  }
  
  tws_delay(10);
}


void i2c_scan()
{
  cmd = 0;
  val = 0;
  if (TinyWireS.available()){
    cmd = TinyWireS.receive(); // receive byte as a character
    val = TinyWireS.receive();    // receive byte as an integer
  }
  if (val == 22) {setup();}
}


void blink(byte led, byte times){ // poor man's display
  if (times > 10){times = 0;}
  for (byte i=0; i< times; i++){
    digitalWrite(led,HIGH);
    delay (100);
    digitalWrite(led,LOW);
    delay (100);
  }
}


void fwd(int mtr, int val){
  analogWrite(ENA[mtr], val);
  //digitalWrite(ENA[mtr], HIGH);
  if (mtr){
    digitalWrite(MTR[mtr][0], HIGH);
    digitalWrite(MTR[mtr][1], LOW);
  }
  else{
    digitalWrite(MTR[mtr][1], HIGH);
    digitalWrite(MTR[mtr][0], LOW);
  }
}


void back(int mtr, int val){
  analogWrite(ENA[mtr], val);
  //digitalWrite(ENA[mtr], HIGH);
  if (mtr){
    digitalWrite(MTR[mtr][1], HIGH);
    digitalWrite(MTR[mtr][0], LOW);
  }
  else{
    digitalWrite(MTR[mtr][0], HIGH);
    digitalWrite(MTR[mtr][1], LOW);
  }
}


void off(int mtr){
  digitalWrite(MTR[mtr][0], LOW);
  digitalWrite(MTR[mtr][1], LOW);
}

