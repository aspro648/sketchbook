// http://playground.arduino.cc/Main/RotaryEncoders

/*
Interrupt Example (the Encoder interrupts the processor). Uses both Interrupt pins

Code for reading encoder using 2 interrupts on pin 2 & pin3

Note: the code below uses 2 interrupts to read the full resolution of the encoder. 
The code above used 1 interrupt. It read half the resolution by only checking 
EncoderPin A for position, but it freed up an interrupt pin. 

*/


#define encoder0PinA 2
#define encoder0PinB 3
volatile unsigned int encoder0Pos = 0;

int led = 13;
int mtrA_pos = 14;


void setup() {

  pinMode(encoder0PinA, INPUT); 
  pinMode(encoder0PinB, INPUT); 
  digitalWrite(encoder0PinA, HIGH);
  digitalWrite(encoder0PinB, HIGH);
  
// encoder pin on interrupt 0 (pin 2)

  attachInterrupt(0, doEncoderA, CHANGE);

// encoder pin on interrupt 1 (pin 3)

  attachInterrupt(1, doEncoderB, CHANGE);  

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.begin (9600);
  Serial.println("encoder");

}

int count = 0;
boolean state = false;

void loop(){ //Do stuff here 
  //delay(10);
  digitalWrite(led, HIGH);
  delay(1000);
  Serial.println (encoder0Pos, DEC);
  digitalWrite(led, LOW);
  delay(1000);
  Serial.println (encoder0Pos, DEC);
}

void doEncoderA(){

  // look for a low-to-high on channel A
  if (digitalRead(encoder0PinA) == HIGH) { 

    // check channel B to see which way encoder is turning
    if (digitalRead(encoder0PinB) == LOW) {  
      encoder0Pos = encoder0Pos + 1;         // CW
    } 
    else {
      encoder0Pos = encoder0Pos - 1;         // CCW
    }
  }

  else   // must be a high-to-low edge on channel A                                       
  { 
    // check channel B to see which way encoder is turning  
    if (digitalRead(encoder0PinB) == HIGH) {   
      encoder0Pos = encoder0Pos + 1;          // CW
    } 
    else {
      encoder0Pos = encoder0Pos - 1;          // CCW
    }
  }
  //Serial.println (encoder0Pos, DEC);          
  // use for debugging - remember to comment out

}

void doEncoderB(){

  // look for a low-to-high on channel B
  if (digitalRead(encoder0PinB) == HIGH) {   

   // check channel A to see which way encoder is turning
    if (digitalRead(encoder0PinA) == HIGH) {  
      encoder0Pos = encoder0Pos + 1;         // CW
    } 
    else {
      encoder0Pos = encoder0Pos - 1;         // CCW
    }
  }

  // Look for a high-to-low on channel B

  else { 
    // check channel B to see which way encoder is turning  
    if (digitalRead(encoder0PinA) == LOW) {   
      encoder0Pos = encoder0Pos + 1;          // CW
    } 
    else {
      encoder0Pos = encoder0Pos - 1;          // CCW
    }
  }

} 
