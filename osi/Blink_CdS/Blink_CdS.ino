/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int red = 5;
int yellow = 6;
int green = 7;
int CdS = 0;
int button = 1;
int cycle = false;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(red, OUTPUT);     
  pinMode(yellow, OUTPUT); 
  pinMode(green, OUTPUT); 
  pinMode(button, INPUT);
  attachInterrupt(button, swap, CHANGE);
}

void swap(){
  cycle = !cycle;
  delay(500); 
}

// the loop routine runs over and over again forever:
void loop() {
  if (!cycle){
    int light = analogRead(CdS);
    digitalWrite(red, LOW);
    digitalWrite(yellow, LOW);
    digitalWrite(green, LOW);
    if (light > 800){
      digitalWrite(red, HIGH);   // turn the LED on (HIGH is the voltage level)
    }
    else if(light > 600) {
      digitalWrite(yellow, HIGH); 
    }
    else if (light > 200) {
      digitalWrite(green, HIGH);
    }

    else{
      stoplight(); 
    }
  }

}


void stoplight() {
  digitalWrite(green, HIGH);
  delay(1000);
  digitalWrite(green, LOW);
  digitalWrite(yellow, HIGH);
  delay(1000);
  digitalWrite(yellow, LOW);
  digitalWrite(red, HIGH);
  delay(1000);
  digitalWrite(red, LOW);
}
