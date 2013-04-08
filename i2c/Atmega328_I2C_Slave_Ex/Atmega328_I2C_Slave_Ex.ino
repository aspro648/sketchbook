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


#include "TinyWireS.h"                  // wrapper class for I2C slave routines

#define I2C_SLAVE_ADDR  0x26            // i2c slave address (38)
#define LED_PIN         3              // ATtiny Pin ?

char cmd;
int val;


void setup(){
  pinMode(LED_PIN, OUTPUT);             // for general DEBUG use
  blink(LED_PIN, 2);                    // show it's alive
  TinyWireS.begin(I2C_SLAVE_ADDR);      // init I2C Slave mode
}


void loop(){
  readI2C(); // check for message
  if (cmd != 'Z'){
    blink(LED_PIN, val);
    sendI2C(val);
  }
  delay(10);
}


void readI2C(){
  // If available, should get a char comand and byte value
  cmd = 'Z';
  val = 0;
  while(1 < TinyWireS.available())  // loop through all but the last
  {
    cmd = TinyWireS.receive(); // receive byte as a character
    //Serial.println(cmd);         // print the character
  }
  val = TinyWireS.receive();   // receive byte as an integer  
}


void sendI2C(byte send_val){
  TinyWireS.send(send_val);
}


void blink(byte led, byte times){ // poor man's display
  if (times > 100){times = 1;}
  for (byte i=0; i< times; i++){
    digitalWrite(led, HIGH);
    delay (100);
    digitalWrite(led, LOW);
    delay (100);
  }
}


