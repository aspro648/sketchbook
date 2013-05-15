/*

Sketch to demonstrate developing code on Arduino and transfering it to
an Attiny.

Requires attiny core from http://hlt.media.mit.edu/?p=1695

Attiny 25/45/85 Pinouts
                  +-\/-+
 Ain0 (D 5) PB5  1|    |8  Vcc
 Ain3 (D 3) PB3  2|    |7  PB2 (D 2)  Ain1
 Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1
            GND  4|    |5  PB0 (D 0) pwm0
                  +----+

Using a standard LED as output and a
TEPT4400 ambient light sensor with a 1M ohm resistor

Makersbox.blogspot.com
648.ken@gmail.com

*/
 

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  #define ARDUINO 0
  #define led_pin 1 // pwm
  #define led_gnd 2 
#else  
  #define ARDUINO 1
  #define led_pin 6 // pwm
  #define led_gnd 7 
#endif

#define sensor_pin 3 // Analog 3
#define sensor_sup 5
#define sensor_gnd 4


int timeout = 10000; // about 6 minutes
int threshold = 100;
int counter = 0;
boolean LED = LOW;
int fade = 25;
int fadeAmount = 5;


void flashLED(int num){
  for (int x=0; x < num; x++){
    analogWrite(led_pin, 25);
    delay(250);
    digitalWrite(led_pin, LOW);
    delay(250);    
  }
}


void setup() {                
  pinMode(led_gnd, OUTPUT);
  digitalWrite(led_gnd, LOW);
  
  pinMode(sensor_sup, OUTPUT);
  digitalWrite(sensor_sup, HIGH);

  pinMode(sensor_gnd, OUTPUT);
  digitalWrite(sensor_gnd, LOW);
  
  flashLED(2);
  #if(ARDUINO)
    Serial.begin(9600);
  #endif
}


void loop() {
  int val = analogRead(sensor_pin);
  //int val = digitalRead(sensor_pin);
  if (val < threshold){
    LED = HIGH;
  }
  else{
    LED = LOW;
    counter = 0;
    fade = 5;
    fadeAmount = random(5) + 5;
  }
  if (LED && counter < timeout){
    analogWrite(led_pin, fade);
  }
  else{
    digitalWrite(led_pin, LOW);
  }
  fade = fade + fadeAmount;
  if (fade > 200 || fade < 0){
    fadeAmount = -fadeAmount;
    if(fade < 0){
      fade = 0;
      int d = random(750) + 250;
      delay(d);
    }
  }

  #if(ARDUINO) 
    Serial.print(val);
    Serial.print(" ");
    Serial.print(counter);
    Serial.println();
  #endif

  counter ++;
  delay(25);
}
