

#define A 0
#define B 1

int motor_pwm = 8;           //atmega pin14
int motor_pin_1 = 11;         //atmega pin17
int motor_pin_2 = 12;         //atmega pin16
int led_pin = 13;            //atmega pin19


void setup() {
  Serial.begin(9600);
  Serial.println("motor_control.ino");
  pinMode(motor_pwm, OUTPUT);
  pinMode(motor_pin_1, OUTPUT);
  pinMode(motor_pin_2, OUTPUT);
  pinMode(led_pin, OUTPUT);
  analogWrite(motor_pwm, 255);
  digitalWrite(motor_pin_1, LOW);
  digitalWrite(motor_pin_2, LOW);
  digitalWrite(led_pin, LOW);
}

void loop() {
  
  digitalWrite(motor_pin_1, HIGH);
  //digitalWrite(led_pin, HIGH);
  delay(500);
  digitalWrite(motor_pin_1, LOW);
  //digitalWrite(led_pin, LOW);
  delay(5000);
  digitalWrite(motor_pin_2, HIGH);
  //digitalWrite(led_pin, HIGH);
  delay(500);
  digitalWrite(motor_pin_2, LOW);
  //digitalWrite(led_pin, LOW);
  delay(5000); 
}
