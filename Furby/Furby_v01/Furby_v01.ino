/*
Hacking a 2005 model Furby!.
Furby_v00 controls just the motors.
Furby_v01 Adding intterupt control and encoder reading
          (see encoder.ino and PinChangeInt.ino). 

The 2005 Furby includes two motors, an upper one 
for the eye/ears/mouth and a lower one for the legs 
to rock the body back and forth. Both motors have 
cams with position indicator switches to help determine
their home position. The upper motor also includes 
includes an optical rotary encoder to help determine 
where you are in the rotation. We need to handle 
these signals expediently, so need to use interrupts.

Encoder code from http://playground.arduino.cc/Main/RotaryEncoders
PinChangeInt lib from http://code.google.com/p/arduino-pinchangeint/


  D0   RX
  D1   TX
  D2   encoder0PinA
 *D3   encoder0PinB
  D4   upperMtr Home (hardware 20ms debounced)
 *D5   lowerMtr A 
 *D6   lowerMtr B
  D7   
  D8   
 *D9   upperMtr ENABLE 
  D10  lowerMtr ENABLE 
 *D11  upperMtr A
 *D12  upperMtr B
  D13  
  A0
  A1
  A2
  A3  
  A4 IC2 DAT 
  A5 IC2 CLK 

*/


#include <PinChangeInt.h>

int led = 13;
int dir = 0;
int maxRange = 850;

// MOTOR VARBS {enable, pin 1, pin 2}
int mtr[][3] = {{9, 11, 12}, {10, 5, 7}};

// ENCODER VARBS and ROUTINES
#define encoder0PinA 2
#define encoder0PinB 3
#define upperMtrHome 4
volatile int encoder0Pos = 0;

void mtrHome() {
  // pin change detected, reset encoder0Pos)
  Serial.print(encoder0Pos);
  Serial.println(" - > reset");
  encoder0Pos = 0;
}


void goHome(){
  while(!digitalRead(upperMtrHome)){
      forward(1);  
  }
  off(1);
  encoder0Pos = 0;
  Serial.println("found home");
}


void moveRange(int low, int high, int reps){

  if(encoder0Pos < low){
    while (encoder0Pos < low){
      forward(1);
    }
  }
  else{
    while (encoder0Pos > low){
      reverse(1);
    }
  }
  
  for(int x = 0; x < reps; x++){
    while (encoder0Pos < high){
      forward(1);
      delay(50);
      off(1);
      delay(50);
    }   
    while (encoder0Pos > low){
      reverse(1);
      delay(50);
      off(1);
      delay(50);
    }   
  }
  off(1);

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
  if(encoder0Pos < 0){
    encoder0Pos = maxRange;
  }
} 


void setup() {           
  Serial.begin(115200);
  Serial.println("Furby_01 code");    

  pinMode(led, OUTPUT); 
  for(int i = 0; i < 2; i++){
    for(int j = 0; j < 3; j++){
      pinMode(mtr[i][j], OUTPUT);
      digitalWrite(mtr[i][j], LOW);
    } 
  }

  for(int i = 0; i < 2; i++){
    analogWrite(mtr[i][0], 125);
  }

  // encoder setup
  pinMode(encoder0PinA, INPUT); 
  pinMode(encoder0PinB, INPUT);
  pinMode(upperMtrHome, INPUT); 
  digitalWrite(encoder0PinA, HIGH); // pull-ups!
  digitalWrite(encoder0PinB, HIGH);
  digitalWrite(upperMtrHome, HIGH);
  PCintPort::attachInterrupt(encoder0PinA, &doEncoderA, CHANGE);
  PCintPort::attachInterrupt(encoder0PinB, &doEncoderB, CHANGE);
  PCintPort::attachInterrupt(upperMtrHome, &mtrHome, FALLING);

  goHome(); // set index for upper motor
  moveRange(50, 150, 4);
}

// the loop routine runs over and over again forever:
void loop() {
  Serial.print(digitalRead(upperMtrHome));
  Serial.print(" ");
  Serial.println(encoder0Pos);
  int m = 1;
  reverse(m);
  //forward(1);
  delay(50);               // wait for a second
  off(m);
  delay(200);               // wait for a second
  //reverse(m);
  //reverse(1);
  //delay(50);               // wait for a second
  //off(m);
  //off(1);
  //delay(10);               // wait for a second
}


void forward(int m) {
  //Serial.print("forward ");
  digitalWrite(led, HIGH);
  digitalWrite(mtr[m][1], HIGH);
  digitalWrite(mtr[m][2], LOW);
  dir = 1; 
}


void reverse(int m) {
  //Serial.print("reverse ");
  digitalWrite(led, LOW);
  digitalWrite(mtr[m][1], LOW);
  digitalWrite(mtr[m][2], HIGH);
  dir = -1; 
}


void off(int m) {
  //Serial.println("off");
  digitalWrite(mtr[m][1], LOW);
  digitalWrite(mtr[m][2], LOW);
  dir = 0;
}
