/*
 * WiiChuk Train Controller
 * Ken Olsen (648.ken@gmail.com)
 * http://www.dawson-station.blogspot.com
 * Mar 2010
 *
 * Uses LadyAda motor shield (www.ladyada.net/make/mshield/)
 * and Tod E. Kurt's brilliant WiiMote Connector and libary:  
 * http://todbot.com/blog/2008/02/18/wiichuck-wii-nunchuck-adapter-available/
 *
 Hardware:
 
  D0   RX
  D1   TX
  D2   Sound DAC
 *D3   Sound DAC
  D4   Sound DAC
 *D5   Sound DAC
 *D6   LED Warn 4
  D7   LED Spur 0
  D8   LED Spur 1
 *D9   LED Spur 2
  D10  Sound SD IO 
 *D11  Sound SD IO
 *D12  Sound SD IO
  D13  Sound SD IO
  A0/D14 LED Warn 2
  A1/D15 LED Warn 1
  A2/D16 LED Warn 0
  A3/D17 Button input
  A4/D18 LED Front
  A5/D19 LED Button
 
 v4 - add night sounds! toggles between each. Moved LED routine to led_sub()
 
 */

#include <AF_Wave.h>
#include <avr/pgmspace.h>
#include "util.h"
#include "wave.h"

boolean debug = true;

// Sound varbs
AF_Wave card;
File f;
Wavefile wave;      // only one!
uint16_t samplerate;
uint8_t tracknum = 0;

// button varbs
int buttonPin = 17;
int buttonLED = 19;
int ledPin =  18;    
byte inByte;
int ledByte;
//int ledPins[] = { 6,7,8,9,14,15,16 };
int ledPins[] = { 7, 8, 9, 16, 15, 14, 6 };
int LEDVal[] = { 0, 1, 0, 0, 0, 0, 0 }; // leds to light

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
int sendFlag = 0; // track if serial message send
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
int day = 0;
int lastday = 1;


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  if (debug) {Serial.println("Wave test!");}

  // setup pins
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  pinMode(buttonLED, OUTPUT);
  digitalWrite(buttonLED, LOW);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH); // turn on pull-up 
  Serial.flush(); // clear buffer
  Serial.println(buttonPin);
  for (int pin = 0; pin < 7; pin++){
    pinMode(ledPins[pin], OUTPUT);
    digitalWrite(ledPins[pin], HIGH);
  } 
  delay(500);
  for (int pin = 0; pin < 7; pin++){
    digitalWrite(ledPins[pin], LEDVal[pin]);
  }   
  digitalWrite(ledPin, LOW);

  // setup SD pins
  pinMode(2, OUTPUT); 
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  if (!card.init_card()) {
    if (debug) {putstring_nl("Card init. failed!"); return;}
  }
  if (!card.open_partition()) {
    if (debug) {putstring_nl("No partition!"); return;}
  }
  if (!card.open_filesys()) {
    if (debug) {putstring_nl("Couldn't open filesys"); return;}
  }

 if (!card.open_rootdir()) {
    if (debug) {putstring_nl("Couldn't open dir"); return;}
  }

  if (debug) {putstring_nl("Files found:");}
  ls();
}


void ls() {
  char name[13];
  int ret;
  
  card.reset_dir();
  if (debug) {putstring_nl("Files found:");}
  while (1) {
    ret = card.get_next_name_in_dir(name);
    if (!ret) {
       card.reset_dir();
       return;
    }
    if (debug) {Serial.println(name);}
  }
}


void loop() { 
  uint8_t i, r; //??
  char c, name[15]; //??
  led_sub();

  int reading = digitalRead(buttonPin); // read button
  //Serial.println(reading);
  if (reading != lastButtonState) { // switch change
    lastDebounceTime = millis(); // reset the debouncing timer
  } 
  //Serial.println(buttonState);
  if ((millis() - lastDebounceTime) > debounceDelay) {
    buttonState = reading; // long enough
    //Serial.println(buttonState);
    digitalWrite(buttonLED, !buttonState);
    if (buttonState == 0 && sendFlag == 0) {

      sendFlag = 1;
      playcomplete("MILL_BKG.WAV"); // start sound
    }
    if (buttonState == 1) {
      sendFlag = 0; // reset flag
    }
  }
  playcomplete("n");
  
  //digitalWrite(ledPin, buttonState); // light LED
  lastButtonState = reading;  // save reading

}

void playcomplete(char *name) {
  uint16_t potval;
  uint32_t newsamplerate;
  //char* fname[] = {"NIGHT.WAV", "MILL.WAV"};
  char* fname[] = {"NIGHT.WAV", "MILL_L.WAV"};
  char* outStr[] = {"b", "a"};

  Serial.print(outStr[day]); // send signal to other board
  playfile(fname[day]);  

  /*
  if (day) {
    Serial.print("a"); // send signal to other board
    playfile("MILL_BKG.WAV");
  }
  else {
    Serial.print("b"); // signal to stop
    playfile("NIGHT.WAV");
  }*/

  digitalWrite(buttonLED, day); // turn it off
  samplerate = wave.dwSamplesPerSec;
  delay(1000); //??
  while (wave.isplaying) {     
	// you can do stuff here!
        led_sub();
        sendFlag = 0;
        int reading = digitalRead(buttonPin); // read button
      
        if (reading != lastButtonState) { // switch change
          lastDebounceTime = millis(); // reset the debouncing timer
          //Serial.println("reset lastDebounce");
        } 
        //Serial.print(reading); Serial.print(" ");Serial.print(buttonState);
        //Serial.print(" "); Serial.println(sendFlag);        
        if ((millis() - lastDebounceTime) > debounceDelay) {
          buttonState = reading; // long enough

          //digitalWrite(buttonLED, !buttonState);
          if (reading == 0 && sendFlag == 0) {
            sendFlag = 1;
            //if (day){
              //Serial.print("b"); // send signal to other board
            //}
            day = !day;
            digitalWrite(buttonLED, LOW); // turn it off
            wave.stop();
            card.close_file(f);
            playcomplete("n");
            return;
          }
          if (buttonState == 1) {
            sendFlag = 0; // reset flag
          }
        }
	//delay(500);
   }
  if (day) {
    day = !day;
  }
  card.close_file(f);
  playcomplete("n");
}

void playfile(char *name) {
   f = card.open_file(name);
   if (!f) {
      if (debug) {putstring_nl(" Couldn't open file"); return; }
   }
   if (!wave.create(f)) {
     if (debug) {putstring_nl(" Not a valid WAV"); return;}
   }
   // ok time to play!
   wave.play();
}

void led_sub(){
  if (Serial.available() > 0) { // read the incoming byte:
    inByte = Serial.read();
    Serial.print("inByte: ");
    Serial.println(inByte, DEC);
    if (inByte >= 128){ // valid LED code received
      inByte = inByte - 128; // drop first digit
      for (int x = 0; x < 7; x++){
        LEDVal[x] = (inByte >> x) & 1;
        digitalWrite(ledPins[x], LEDVal[x]);
        Serial.print(LEDVal[x]);
      }
      Serial.println();
    }
    if (inByte = 'a') { // signal to start mill sounds
      if (!day){
        day = true; // make it day
        wave.stop();
        card.close_file(f); // close current file
        playcomplete("n");  // play day      
      }
    }
  }
}

void LED(){
  for (int x = 0; x < 7; x++){
    LEDVal[x] = (inByte >> x) & 1;
    if (day){
      digitalWrite(ledPins[x], LEDVal[x]);
      Serial.print(LEDVal[x]);
    }
    else {
      digitalWrite(ledPins[x], 0);
    }
  }
  Serial.println();
}
