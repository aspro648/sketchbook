/*
.
 */

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:

int led = 13;
int dir = 0;
int motor_pwm = 9;           
int motor_pin_1 = 11;         
int motor_pin_2 = 12;       
int motor_ind = 14; ///a0

// {enable, pin 1, pin 2}
int mtr[][3] = {{9, 11, 12}, {10, 5, 7}};

int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT); 
  for(int i = 0; i < 2; i++){
    for(int j = 0; j < 3; j++){
      pinMode(mtr[i][j], OUTPUT);
      digitalWrite(mtr[i][j], LOW);
    } 
  }

  for(int i = 0; i < 2; i++){
    analogWrite(mtr[i][0], 255);
  }
  
  pinMode(motor_ind, INPUT);
  digitalWrite(led, LOW);
  digitalWrite(motor_ind, HIGH); // pullup
  Serial.begin(9600); 
}

// the loop routine runs over and over again forever:
void loop() {
  Serial.print("BLINK ");
  int m = 0;
  forward(m);
  forward(1);
  delay(1000);               // wait for a second
  off(m);
  delay(500);               // wait for a second
  reverse(m);
  reverse(1);
  delay(500);               // wait for a second
  off(m);
  off(1);
  delay(100);               // wait for a second
}


void forward(int m) {
  Serial.print("forward ");
  digitalWrite(led, HIGH);
  digitalWrite(mtr[m][1], HIGH);
  digitalWrite(mtr[m][2], LOW);
  dir = 1; 
}


void reverse(int m) {
  Serial.print("reverse ");
  digitalWrite(led, LOW);
  digitalWrite(mtr[m][1], LOW);
  digitalWrite(mtr[m][2], HIGH);
  dir = -1; 
}


void off(int m) {
  Serial.println("off");
  digitalWrite(mtr[m][1], LOW);
  digitalWrite(mtr[m][2], LOW);
  dir = 0;
}
