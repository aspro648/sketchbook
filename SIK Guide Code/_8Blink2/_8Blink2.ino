/*
  Blink
 
  This example code is in the public domain.
 */
 
int CdS = 1;
int pins[] = {3, 4, 5, 6, 7, 8, 9, 10 };
boolean OFF = LOW;
boolean ON = HIGH;
// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);  
  // initialize the digital pins as output.
  for(int x=0; x < 8; x++){
    pinMode(pins[x], OUTPUT); 
    digitalWrite(pins[x], OFF);
  } 

  cylon(4);
  
}

int count = 0;
// the loop routine runs over and over again forever:
void loop() {
  int val = analogRead(CdS);
  val = analogRead(CdS);
  int out = map(val, 400, 750, 0, 6);
  Serial.print(count);
  Serial.print(" ");
  Serial.print(val);
  Serial.print(" ");
  Serial.println(out);
  level(out);
  delay(10);
  if (count == 1000){
    count = 0;
    cylon(1);
  }
  count++;
}

void chase(int i){

  for(int x=0; x < 8; x++){
    digitalWrite(pins[x], ON);
    delay(i);
    digitalWrite(pins[x], OFF);
  } 
}


void rev_chase(int i){

  for(int x=7; x >= 0; x--){
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

void cylon(int rep){
  for (int x=rep; x > 0; x=x-2){
    chase(x * 3);
    rev_chase(x * 30); 
  }
  for (int x=0; x < rep; x=x+2){
    chase(x * 30);
    rev_chase(x * 30); 
  }  
}
