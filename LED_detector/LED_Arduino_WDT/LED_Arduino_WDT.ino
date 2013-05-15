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
  #define led_pin 5 // pwm
  #define led_gnd 7 
#endif

#define sensor_pin 3 // Analog 3
#define sensor_sup 5
#define sensor_gnd 4

#include <avr/sleep.h>
#include <avr/wdt.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


int timeout = 10000; // about 6 minutes
int threshold = 120;
int counter = 0;
boolean LED = LOW;
int fade = 25;
int fadeAmount = 5;
volatile boolean f_wdt = 1;


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

  setup_watchdog(8); // approximately 4 seconds sleep
  
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

// set system into the sleep state 
// system wakes up when wtchdog is timed out
void system_sleep() {
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System sleeps here

  sleep_disable();                     // System continues execution here when watchdog timed out 
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}


// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {

  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;

  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}
  
// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  f_wdt=1;  // set global flag
}
