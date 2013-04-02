

// PIR
// http://bildr.org/2011/06/pir_arduino/

// TIP120
// http://bildr.org/2011/03/high-power-control-with-arduino-and-tip120/


int pirPin = 2; //digital 2
int mtrPin = 3;
int ledPin = 13;


void setup(){
 Serial.begin(9600); 
 pinMode(pirPin, INPUT);
 digitalWrite(pirPin, HIGH); // pull up resistor
 pinMode(ledPin, OUTPUT);
 pinMode(mtrPin, OUTPUT);

}


void loop(){
  int pirVal = digitalRead(pirPin);

  if(pirVal == LOW){ //was motion detected
    Serial.println("Motion Detected"); 
    digitalWrite(ledPin, HIGH);
    digitalWrite(mtrPin, HIGH);
    delay(1000); 
    digitalWrite(ledPin, LOW);
    digitalWrite(mtrPin, LOW);
  }

}
