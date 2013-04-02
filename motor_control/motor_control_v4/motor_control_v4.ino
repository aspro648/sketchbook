/*

i2c_motor_v2 layout

http://www.gabrielbianconi.com/projects/arduinonunchuk/
nunchuk.analogX
nunchuk.analogY
nunchuk.accelX
nunchuk.accelY
nunchuk.accelZ
nunchuk.zButton
nunchuk.cButton
 */

#include <Wire.h>
#include <ArduinoNunchuk.h>
#define A 0
#define B 1


int turnout_pwm[] = {3, 5};   // pins for pwm
int turnout[][2] = {{4, 2}, {7, 6}}; // pins for turnouts
int turnout_pos[] = {0, 0};   //inital positions

int motor_pwm = 8;           //atmega pin17
int motor_pin_1 = 11;         //atmega pin18
int motor_pin_2 = 12;         //atmega pin19
int pwm = 500;                // 0 - 1023
int throttle_trim_pin = 0;    // a0 atmega pin23
int momentum_trim_pin = 1;    // a2 atmega pin24
int dir = 0;
int throttle;

boolean chuk;
ArduinoNunchuk nunchuk = ArduinoNunchuk();
int chukXcenter; // stores inital value
int chukX;

void setup() {
  Serial.begin(9600);
  Serial.println("motor_control.ino");
  initChuk();
  pinMode(motor_pwm, OUTPUT);
  pinMode(motor_pin_1, OUTPUT);
  pinMode(motor_pin_2, OUTPUT);
  pinMode(throttle_trim_pin, INPUT);
  pinMode(momentum_trim_pin, INPUT);
  for(int x=0; x<2; x++){
    pinMode(turnout_pwm[x], OUTPUT);
    digitalWrite(turnout_pwm[x], HIGH);
    for(int y = 0; y<2; y++){
      pinMode(turnout[x][y], OUTPUT);
      digitalWrite(turnout[x][y], LOW);
    };
  };
  analogWrite(motor_pwm, 0);
  digitalWrite(motor_pin_1, LOW);
  digitalWrite(motor_pin_2, LOW);
}


boolean initChuk(){
  // initialize check and check inital readings
  nunchuk.init();
  delay(100);
  nunchuk.update();
  Serial.println(nunchuk.analogX, DEC);
  if (nunchuk.analogX > 255 or nunchuk.analogX <= 0){
    Serial.println("no chuk?");
    chuk = false;
    return false;
  };
  chukXcenter = nunchuk.analogX;
  Serial.print("Chuck initalized, centerX = ");
  Serial.println(chukXcenter);
  chuk = true;
  delay(1000);
  return true;
}


boolean checkChuk(){
  nunchuk.update();
  //Serial.println(nunchuk.analogX);
  if (nunchuk.analogX > 255 or nunchuk.analogX <= 0){
    //Serial.println("no chuk?");
    chuk = false;
    return false;
  }
  else if (int(nunchuk.analogX) == 255) {
    initChuk();
  }
  chukX = nunchuk.analogX - chukXcenter;
  return true;
}


void forward() {
  Serial.print("forward ");
  Serial.println(pwm);
  digitalWrite(motor_pin_1, HIGH);
  digitalWrite(motor_pin_2, LOW);
  //analogWrite(motor_pwm, pwm);
  dir = 1; 
}


void reverse() {
  Serial.print("reverse ");
  Serial.println(pwm);
  digitalWrite(motor_pin_1, LOW);
  digitalWrite(motor_pin_2, HIGH);
  //analogWrite(motor_pwm, pwm);
  dir = -1; 
}


void off() {
  Serial.println("off");
  digitalWrite(motor_pin_1, LOW);
  digitalWrite(motor_pin_2, LOW);
  dir = 0;
}


void loop() {
  int throttle_limit = 1024 - analogRead(throttle_trim_pin);
  int momentum_limit = 1024 - analogRead(momentum_trim_pin);
  checkChuk();
  Serial.print(throttle_limit);
  Serial.print(" ");
  Serial.print(momentum_limit);
  Serial.print(" ");
  Serial.print(dir);
  Serial.print(" ");
  Serial.print(chuk, DEC);
  Serial.print(" ");
  Serial.print(nunchuk.analogX);
  Serial.print(" ");
  Serial.print(chukX);
  Serial.print(" ");  
  Serial.println(throttle);
  if (chuk) {
    if (chukX > 10 && dir != 1){
      forward();
    }
    else if (chukX < -10 && dir != -1){
      reverse();
    }
    if (abs(chukX) <= 10 && dir != 0) {
      off();
    }
    throttle = abs(chukX) / 100.0 * throttle_limit / 1023 * 255;
    if (throttle > 255) {
      throttle = 255;
    }
    analogWrite(motor_pwm, throttle);
    if (nunchuk.cButton){
      switch_turnout(A);
    }
    if (nunchuk.zButton){
      switch_turnout(B);
    } 
    
  }

  //int throttle_limit = 1023;
  //Serial.print(throttle_limit);
  //Serial.print(" ");
  //Serial.println(momentum_limit);
  //delay(100);
  //forward(throttle_limit);
  //delay(10000);
  //off();
  //delay(5000);
  //reverse(throttle_limit);
  //delay(5000);
  //off();
  delay(10);
  //loop_turnouts();  
}


void switch_turnout(int turn){
  if(turnout_pos[turn] == 0){
    digitalWrite(turnout[turn][0], HIGH);
    delay(400);
    digitalWrite(turnout[turn][0], LOW);
    turnout_pos[turn] = 1;
    Serial.print("turnout ");
    Serial.print(turn);
    Serial.print(" moved to 1");
    delay(400);
  }
  else{
    digitalWrite(turnout[turn][1], HIGH);
    delay(200);
    digitalWrite(turnout[turn][1], LOW);
    turnout_pos[turn] = 0;   
    Serial.print("turnout ");
    Serial.print(turn);
    Serial.print(" moved to 0");
    delay(500);
  }
}
 
  


void loop_turnouts() {
  Serial.println("1");
  //digitalWrite(turnout_B_1, HIGH);
  delay(100);
  //digitalWrite(turnout_B_1, LOW);
  delay(2000);
  //digitalWrite(turnout_B_2, HIGH);
  delay(100);
  //digitalWrite(turnout_B_2, LOW);
  delay(2000);   
 
}
