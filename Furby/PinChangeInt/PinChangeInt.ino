
// Furby '05 includes two motors. 
// Both motors have cams with position indicator switches to help determine
// there position. The upper motor includes an also includes a
// optical rotary encoder to help determine where you are in the rotation.
// We need to handle these signals expediently, so lets us intterupts:

// http://code.google.com/p/arduino-pinchangeint/
#include <PinChangeInt.h>

// encoder code from:
// http://playground.arduino.cc/Main/RotaryEncoders

#define encoder0PinA 2
#define encoder0PinB 3
#define upperMtrHome 4
volatile unsigned int encoder0Pos = 0;


void mtrHome() {
  // falling signal detected, reset encoder0Pos)
  Serial.print(encoder0Pos);
  Serial.print(" - > reset");
  encoder0Pos = 0;
}


void doEncoderA(){
  // look for a low-to-high on channel A
  if (digitalRead(encoder0PinA) == HIGH) { 
    // check channel B to see which way encoder is turning
    if (digitalRead(encoder0PinB) == LOW) {  
      encoder0Pos = encoder0Pos + 1;         // CW
    } 
    else {
      encoder0Pos = encoder0Pos - 1;         // CCW
    }
  }
  else { // must be a high-to-low edge on channel A    
    // check channel B to see which way encoder is turning  
    if (digitalRead(encoder0PinB) == HIGH) {   
      encoder0Pos = encoder0Pos + 1;          // CW
    } 
    else {
      encoder0Pos = encoder0Pos - 1;          // CCW
    }
  }
}

void doEncoderB(){
  // look for a low-to-high on channel B
  if (digitalRead(encoder0PinB) == HIGH) {   
   // check channel A to see which way encoder is turning
    if (digitalRead(encoder0PinA) == HIGH) {  
      encoder0Pos = encoder0Pos + 1;         // CW
    } 
    else {
      encoder0Pos = encoder0Pos - 1;         // CCW
    }
  }
  else {   // Look for a high-to-low on channel B
    // check channel B to see which way encoder is turning  
    if (digitalRead(encoder0PinA) == LOW) {   
      encoder0Pos = encoder0Pos + 1;          // CW
    } 
    else {
      encoder0Pos = encoder0Pos - 1;          // CCW
    }
  }
} 


void setup() {
  Serial.begin(115200);
  Serial.println("Furby encoder and motor home demo code");  
  pinMode(encoder0PinA, INPUT); 
  pinMode(encoder0PinB, INPUT);
  pinMode(upperMtrHome, INPUT); 
  digitalWrite(encoder0PinA, HIGH); // pull-ups!
  digitalWrite(encoder0PinB, HIGH);
  digitalWrite(upperMtrHome, HIGH);
  PCintPort::attachInterrupt(encoder0PinA, &doEncoderA, CHANGE);
  PCintPort::attachInterrupt(encoder0PinB, &doEncoderB, CHANGE);
  PCintPort::attachInterrupt(upperMtrHome, &mtrHome, FALLING);

}


void loop() {
  Serial.println(encoder0Pos);
  delay(250);
}

