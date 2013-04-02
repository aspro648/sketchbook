// assign your variables
int green_led = 11;

// the setup routine runs once when you press reset:
void setup() {                
  pinMode(green_led, OUTPUT);     
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(green_led, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(1000);                   // wait for a second
  digitalWrite(green_led, LOW);  // turn the LED off by making the voltage LOW
  delay(1000);                   // wait for a second
}


