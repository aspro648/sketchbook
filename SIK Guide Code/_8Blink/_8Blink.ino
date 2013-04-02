/*
  Blink
 
  This example code is in the public domain.
 */
 
int CdS = 1;
int pins[] = {3, 4, 5, 6, 7, 8, 3, 10, 11,10, 11, 10, 9, 11,8,3, 7, 6, 5, 4, 3, };
boolean OFF = LOW;
boolean ON = HIGH;
// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);  
  // initialize the digital pins as output.
  for(int x=0; x < 11; x++){
    pinMode(pins[x], OUTPUT); 
    digitalWrite(pins[x], OFF);
  } 
  chase();  
}

// the loop routine runs over and over again forever:
void loop() {
  int val = analogRead(CdS);
  int out = map(val, 600, 850, 0, 6);
  Serial.print(val);
  Serial.print(" ");
  Serial.println(out);
  level(out);
}

void chase(){
  int i = 100;
  for(int x=0; x < 8; x++){
    digitalWrite(pins[x], ON);
    delay(i);
    digitalWrite(pins[x], OFF);
  } 
}

void level(int val){
  for(int x=0; x < 8; x++){
    if(x < val){
      digitalWrite(pins[x], OFF);}
    else{
      digitalWrite(pins[x], ON);} 
    }
}
