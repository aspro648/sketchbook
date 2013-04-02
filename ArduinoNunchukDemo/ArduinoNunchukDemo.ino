/*
 * ArduinoNunchuk Demo
 * 
 * Copyright 2011-2012 Gabriel Bianconi, http://www.gabrielbianconi.com/
 *
 * Project URL: http://www.gabrielbianconi.com/projects/arduinonunchuk/
 * 
 */

#include <Wire.h>
#include <ArduinoNunchuk.h>

#define BAUDRATE 9600

ArduinoNunchuk nunchuk = ArduinoNunchuk();

void setup()
{
  Serial.begin(BAUDRATE);
  Serial.println("ArduinoNunchukDemo");
  nunchuk.init();
  nunchuk.update();
  if (nunchuk.analogX >= 255 or nunchuk.analogX < 0){
    Serial.println("no chuck?");
  }
}

void loop()
{
  nunchuk.update();
  Serial.println(nunchuk.analogX);
  if (nunchuk.analogX > 255 or nunchuk.analogX < 0){
    Serial.println("no chuck?");
  }
  else if (int(nunchuk.analogX) == 255) {
    //setup();
    nunchuk.init();
    Serial.println("nunchuk.init()");
    delay(500);
  } 
  else { 
    Serial.print(nunchuk.analogX, DEC);
    Serial.print(' ');
    Serial.print(nunchuk.analogY, DEC);
    Serial.print(' ');
    Serial.print(nunchuk.accelX, DEC);
    Serial.print(' ');
    Serial.print(nunchuk.accelY, DEC);
    Serial.print(' ');
    Serial.print(nunchuk.accelZ, DEC);
    Serial.print(' ');
    Serial.print(nunchuk.zButton, DEC);
    Serial.print(' ');
    Serial.println(nunchuk.cButton, DEC);
  }
  delay(500);
}
