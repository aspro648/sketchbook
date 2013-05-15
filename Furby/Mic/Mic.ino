/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  Serial.begin(57600);
  pinMode(A0, OUTPUT); 
  digitalWrite(A0, LOW);
  pinMode(A1, INPUT);
  //digitalWrite(A1, HIGH);  
}

// the loop routine runs over and over again forever:
void loop() {
  int a = analogRead(A1);
  Serial.println(a);
  delay(100);
  // wait for a second
}
