/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// 6 = red
// 7 = blue
// 8 = green
// 
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 6;
int red = 6;
int blue = 7;
int green = 8;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(red, OUTPUT);
  digitalWrite(red, HIGH);
  pinMode(blue, OUTPUT);
  digitalWrite(blue, HIGH);
  pinMode(green, OUTPUT);
  digitalWrite(green, HIGH);
  
}

// the loop routine runs over and over again forever:
void loop() {
  int lim = analogRead(2);
  int val = analogRead(1);
  if (1){//val > lim){
    analogWrite(red, 0);
    delay(5);
    analogWrite(red, 255);
    
    analogWrite(green, 255);
    delay(5);
    analogWrite(green, 255);
    
    analogWrite(blue, 127);   // turn the LED on (HIGH is the voltage level)
    delay(5);
    analogWrite(blue, 255);
  }
  else{
    analogWrite(red, 255);
    analogWrite(green, 255);
    analogWrite(blue, 255);    // turn the LED off by making the voltage LOW

  }
  
  
}
