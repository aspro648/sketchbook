/*
 Debounce
 
 Each time the input pin goes from LOW to HIGH (e.g. because of a push-button
 press), the output pin is toggled from LOW to HIGH or HIGH to LOW.  There's
 a minimum delay between toggles to debounce the circuit (i.e. to ignore
 noise).  
 
 The circuit:
 * LED attached from pin 13 to ground
 * pushbutton attached from pin 2 to +5V
 * 10K resistor attached from pin 2 to ground
 
 * Note: On most Arduino boards, there is already an LED on the board
 connected to pin 13, so you don't need any extra components for this example.
 
 
 created 21 November 2006
 by David A. Mellis
 modified 30 Aug 2011
 by Limor Fried
 
This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/Debounce
 */

// PIR
// http://bildr.org/2011/06/pir_arduino/

// TIP120
// http://bildr.org/2011/03/high-power-control-with-arduino-and-tip120/

// constants won't change. They're used here to
// set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
const int mtrPin = 3;
const int ledPin =  13;      // the number of the LED pin
int mtrVal = 200;

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
boolean mtrFlag = false;
long mtrTimer = 0;

void setup() {
  pinMode(14, INPUT);
  digitalWrite(14, HIGH); // pullup on A0
  pinMode(ledPin, OUTPUT);
  pinMode(mtrPin, OUTPUT);
  Serial.begin(9600);
  digitalWrite(ledPin, HIGH);
  Serial.println("\nBUBBLES");
  delay(1000);
  digitalWrite(ledPin, LOW);
}


void loop() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(14);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
 
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    buttonState = !reading;
    //mtrFlag = !mtrFlag;
    if (buttonState){
      if (mtrFlag && mtrTimer > 100){
        mtrFlag = false;
        mtrTimer = 0;
        digitalWrite(mtrPin, LOW);
        digitalWrite(ledPin, LOW);
        delay(1000);
      }
      else if (!mtrFlag){
        mtrFlag = true;
        mtrTimer = 0;
      }
    }
  }
  //if (buttonState){// && !mtrFlag){
  //   mtrFlag = !mtrFlag; 
  //   lastDebounceTime = millis();
  //   delay(1);
  //}
    
  // set the LED using the state of the button:
  digitalWrite(ledPin, mtrFlag);

  if (mtrFlag){
    digitalWrite(mtrPin, HIGH);
    //analogWrite(mtrPin, mtrVal);
    mtrTimer += 1;
    if (mtrTimer > 1000){
      mtrFlag = !mtrFlag;
      mtrTimer = 0; 
    }
    delay(10);
  }
  else{
    digitalWrite(mtrPin, LOW); 
  }
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
  Serial.print(reading);
  Serial.print(" ");
  Serial.print(mtrFlag);
  Serial.print(" ");
  Serial.print(buttonState);
  Serial.print(" ");  
  Serial.println(mtrTimer);
  delay(10);
}
