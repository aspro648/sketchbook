
// modified from:
// http://playground.arduino.cc/Learning/LEDSensor
//
// This example shows one way of using an LED as a light sensor.
// You will need to wire up your components as such:
//
//           + digital2
//           |
//           <
//           > 100 ohm resistor
//           <
//           |
//           |
//         -----
//          / \  LED, maybe a 5mm, clear plastic is good
//         -----
//           |
//           |
//           + digital3
//
// What we are going to do is apply a positive voltage at digital2 and
// a low voltage at digital3. This is backwards for the LED, current will
// not flow and light will not come out, but we will charge up the 
// capacitance of the LED junction and the Arduino pin.
//
// Then we are going to disconnect the output drivers from digital2 and
// count how long it takes the stored charge to bleed off through the 
// the LED. The brighter the light, the faster it will bleed away to 
// digital3.
//
// Then just to be perverse we will display the brightness back on the 
// same LED by turning it on for a millisecond. This happens more often
// with brighter lighting, so the LED is dim in a dim room and brighter 
// in a bright room. Quite nice.
//
// (Though a nice idea, this implementation is flawed because the refresh
// rate gets too long in the dark and it flickers disturbingly.)
//
//#define LED_N_SIDE 2
//#define LED_P_SIDE 3
//#define LED2 5

#define LED_N_SIDE 3
#define LED_P_SIDE 4

#define LED2_N_SIDE 2
#define LED2_P_SIDE 1

#define TIMEOUT 500
#define THRESHOLD 10000 //7000
#define DEADBAND 500 //500


int count = 0;
int LED = LOW;


void setup()
{
  //Serial.begin(9600);
  pinMode(LED2_P_SIDE, OUTPUT);
  pinMode(LED2_N_SIDE, OUTPUT);
  digitalWrite(LED2_N_SIDE, LOW);
}


void loop()
{
  unsigned int j;
  
  // Apply reverse voltage, charge up the pin and led capacitance
  pinMode(LED_N_SIDE,OUTPUT);
  pinMode(LED_P_SIDE,OUTPUT);
  digitalWrite(LED_N_SIDE,HIGH);
  digitalWrite(LED_P_SIDE,LOW);

  // Isolate the pin 2 end of the diode
  pinMode(LED_N_SIDE,INPUT);
  digitalWrite(LED_N_SIDE,LOW);  // turn off internal pull-up resistor

  // Count how long it takes the diode to bleed back down to a logic zero
  for ( j = 0; j < 20000; j++) {
    if ( digitalRead(LED_N_SIDE)==0) break;
  }
  // You could use 'j' for something useful, but here we are just using the
  // delay of the counting.  In the dark it counts higher and takes longer, 
  // increasing the portion of the loop where the LED is off compared to 
  // the 1000 microseconds where we turn it on.
  
  if (j > THRESHOLD + DEADBAND){
    // Turn the light on for 1000 microseconds
    LED = HIGH;
  }
  else if (j < THRESHOLD - DEADBAND){
    LED = LOW;;
    count = 0;
  }
  if (LED && count < TIMEOUT){
    analogWrite(LED2_P_SIDE, 125);
  }
  else{
    digitalWrite(LED2_P_SIDE, LOW);
  }
  //  pinMode(LED_P_SIDE,OUTPUT);
  //  pinMode(LED_N_SIDE,OUTPUT);
  //  delayMicroseconds(1000);
  //}  // we could turn it off, but we know that is about to happen at the loop() start
  if (count % 100 == 0){
    //Serial.print(j);
    //Serial.print(" ");
    //Serial.print(count);
    //Serial.println();
  }
  count ++;

}
