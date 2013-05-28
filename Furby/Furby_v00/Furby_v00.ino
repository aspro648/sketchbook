/*
  Hacking a 2005 model Furby!
  Furby_v0 controls just the motor.
  MakersBox.blogspot.com
  648.ken@gmail.com
*/

//  L293D Dual H-Bridge Pinouts: 
//
//                     +-\/-+
//          Enable 1  1|    |16  VSS
//          Input 1A  2|    |15  Input 2B
//         Output 1A  3|    |14  Output 2B
//               GND  4|    |13  GND
//               GND  5|    |12  GND
//         Output 1B  6|    |11  Output 2A
//          Input 1B  7|    |10  Input 2A
//                VS  8|    |9   Enable 2
//                     +----+

// Connections:
// Arduino 5V -> H-Bridge Pin 8
// Arduino GND-> H-Bridge Pins 4/5/12/13
// Furby 6V   -> H-Bridge Pin 16
// Furby GND  -> H-Bridge Pins 4/5/12/13 


int led = 13;
int motor_pwm = 10;       // H-Bridge Pin 9  [Enable 2]
int motor_pin_1 = 5;      // H-Bridge Pin 15 [Input 2B]  
int motor_pin_2 = 6;      // H-Bridge Pin 10 [Input 2A]


void setup() {                
  pinMode(led, OUTPUT); 
  pinMode(motor_pwm, OUTPUT);
  pinMode(motor_pin_1, OUTPUT);
  pinMode(motor_pin_2, OUTPUT);
  
  analogWrite(motor_pwm, 150); // enable
  digitalWrite(motor_pin_1, LOW);
  digitalWrite(motor_pin_2, LOW);

  Serial.begin(9600); 
  Serial.println("Furby motor test");
}


void loop() {
  int interval=1000;
  digitalWrite(motor_pin_1, HIGH);
  digitalWrite(led, HIGH);
  delay(interval);
  
  digitalWrite(motor_pin_1, LOW);
  digitalWrite(led, LOW);
  delay(interval);
  
  digitalWrite(motor_pin_2, HIGH);
  digitalWrite(led, HIGH);
  delay(interval/2);
  
  digitalWrite(motor_pin_2, LOW);
  digitalWrite(led, LOW);
  delay(interval);
}

