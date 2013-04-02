/*

http://www.gabrielbianconi.com/projects/arduinonunchuk/
 */

#include <Wire.h>
#include <ArduinoNunchuk.h>

int turnout_pin_1 = 3;
int turnout_pin_2 = 4;

int motor_pwm = 11;           //atmega pin17
int motor_pin_1 = 12;         //atmega pin18
int motor_pin_2 = 13;         //atmega pin19
int pwm = 500;                // 0 - 1023
int throttle_trim_pin = 0;    // a0 atmega pin23
int momentum_trim_pin = 2;    // a2 atmega pin25
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
  pinMode(turnout_pin_1, OUTPUT);
  pinMode(turnout_pin_2, OUTPUT);
  analogWrite(motor_pwm, 0);
  digitalWrite(motor_pin_1, LOW);
  digitalWrite(motor_pin_2, LOW);
  digitalWrite(turnout_pin_1, LOW);
  digitalWrite(turnout_pin_2, LOW);
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
    };
    analogWrite(motor_pwm, throttle);
  };

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


void loop_turnouts() {
  Serial.println("1");
  digitalWrite(turnout_pin_1, HIGH);
  delay(100);
  digitalWrite(turnout_pin_1, LOW);
  delay(2000);
  digitalWrite(turnout_pin_2, HIGH);
  delay(100);
  digitalWrite(turnout_pin_2, LOW);
  delay(2000);   
 
}
