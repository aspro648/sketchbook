/*

CONTROL REV 1

D0 - MTRA_1
D1 - gnd
D2 - gnd
D3 - MTRA_2
D4 -        SCL
D5 - ENA_A 
D6 -        SDA
D7 - ENA_B
D8 - MTRB_1
D9 - nc
D10- MTRB_2
 */


int A = 0;
int B = 1;
int pins[] = {0, 1, 2, 3, 5, 7, 8, 10};
int MTR[2][2] = {
  {0, 3},
  {8, 10}
};
int ENA[2] = {5, 7};
int limit[2] = {100, 100};
void setup() {                
  for(int pin = 0; pin < 8; pin++){
    pinMode(pins[pin], OUTPUT);
    digitalWrite(pins[pin], LOW);
  } 
}


void loop() {
  fwd(A);
  delay(1000);
  back(A);
  delay(1000);
  off(A);
  delay(1000);
  fwd(B);
  delay(1000);
  back(B);
  delay(1000);
  off(B); 
  delay(1000); 
}


void fwd(int mtr){
  analogWrite(ENA[mtr], limit[mtr]);
  //digitalWrite(ENA[mtr], HIGH);
  if (mtr){
    digitalWrite(MTR[mtr][0], HIGH);
    digitalWrite(MTR[mtr][1], LOW);
  }
  else{
    digitalWrite(MTR[mtr][1], HIGH);
    digitalWrite(MTR[mtr][0], LOW);
  }
}


void back(int mtr){
  analogWrite(ENA[mtr], limit[mtr]);
  //digitalWrite(ENA[mtr], HIGH);
  if (mtr){
    digitalWrite(MTR[mtr][1], HIGH);
    digitalWrite(MTR[mtr][0], LOW);
  }
  else{
    digitalWrite(MTR[mtr][0], HIGH);
    digitalWrite(MTR[mtr][1], LOW);
  }
}


void off(int mtr){
  digitalWrite(MTR[mtr][0], LOW);
  digitalWrite(MTR[mtr][1], LOW);
}
