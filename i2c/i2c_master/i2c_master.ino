#include <Wire.h>

#define LED_PIN 13

int val;

void setup()
{
  Serial.begin(9600);
  Serial.println("i2c_master device #8");
  Wire.begin(8); // Start I2C Bus as Master
  Wire.onReceive(receiveEvent); // register event
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}


void loop()
{
  Serial.println("send request to 9");
  Wire.beginTransmission(9); // transmit to device #9
  Wire.write(0);              // sends x 
  Wire.endTransmission();    // stop transmitting
  delay(5000);
}


void receiveEvent(int howMany) {
  Serial.println("receiev Event");
  digitalWrite(LED_PIN, HIGH);
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  val = Wire.read();    // receive byte as an integer
  Serial.println(val);         // print the integer

  delay(500);
  digitalWrite(LED_PIN, LOW);

}
