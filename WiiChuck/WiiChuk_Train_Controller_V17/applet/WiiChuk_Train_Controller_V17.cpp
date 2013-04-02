/*
 * WiiChuk Train Controller
 * Ken Olsen (648.ken@gmail.com)
 * http://www.dawson-station.blogspot.com
 * Mar 2010
 *
 * This is being fixed by Connor Ashcroft. Thanks Ken. :)
 *
 * Uses LadyAda motor shield (www.ladyada.net/make/mshield/)
 * and Tod E. Kurt's brilliant WiiMote Connector and libary:  
 * http://todbot.com/blog/2008/02/18/wiichuck-wii-nunchuck-adapter-available/
 *
 
 V11 ADDING SERIAL SEND FOR TURNOUT POSITION 
 V12 - switching Turnout and CiO pins, add shed lighting
 V16 - changing throwout control to the C button
        cruiseSpeed 60 -> 80
        drag 50 -> 30 
        rollDrag 200 -> 100
        
        
 Hardware:
  D0   RX
  D1   TX
  D2   Lights (shed and mill)
 *D3   TO1
  D4   MC
 *D5   MUX S0 
 *D6   MUX S1 
  D7   MC
  D8   MC
 *D9   MUX S3 
  D10  MUX S4 
 *D11  MC
 *D12  MC
  D13  FRONT RED LED
  A0/D14 To 0
  A1/D15 To 1
  A2/D16 SENS CIO
  A3/D17 
  A4/D18 IC2 DAT (Wii chuk)
  A5/D19 IC2 CLK (Wii chuk)
 
 */

#include <Wire.h>
#include "nunchuck_funcs.h"
#include <AFMotor.h>

#include "WProgram.h"
void setup();
void loop();
int debug = 0;  // 0 = none, 1 = throttle, 2 = sensors
boolean sensors = true;  // bipass sensor code

/*
  "For motors 1 and 2 you can choose 
  MOTOR12_64KHZ, MOTOR12_8KHZ, MOTOR12_2KHZ, or MOTOR12_1KHZ. 
  A high speed like 64KHz wont be audible but a low speed 
  like 1KHz will use less power. Motors 3 & 4 are only possible 
  to run at 1KHz and will ignore any setting given. "
  (My engine seems to sounds better with MOTOR12_2KHZ))
*/

// THROTTLE VARIABLES
AF_DCMotor motor(1, MOTOR12_2KHZ); // create motor #1
byte accx,accy,accz,zbut,cbut,joyx,joyy,joyxMid; // hold Chuck readings
int trainDir = FORWARD; // 1 = FORWARD, 2 = BACKWARD
int throttle = 0;       // adjusted joystick reading
int speedLimit = 100;   // max motor set speed (up to 255)
int speedSet = 0;       // current motor set speed
int deadBand = 10;      // for joystick
float curSpeed = 0;     // current motor speed
float memSpeed;         // remember speed if auto reversing
float diff = 0;         // change in speed based on accel/momentum
int drag = 30;          // lower = faster speed response
int stallSpeed = 20;    // speed needed to start train movement
int brakeDrag = 100;    // lower = slows faster, must be >0
int rollDrag = 100;     // higher accelerates slower
int cruise = false;     // cruise control to hold speed?
int cruiseSpeed = 80;   // speed when in auto mode
int warnSpeed = 0;      // speed if EOT warning
int EOT_trip, EOT_warn; // End Of Track flages

// TURNOUT VARIABLES
int throwFlag = 1;      // allow one throw per 1000 cycles
int throwClear;         // true if throwout not covered
int throwPin[] = { 14, 15 };  // throwout i/o pins
int spur = 1;           // current spur, center = default = 1
int newSpur;            // hold next spur move
int throwPos[3][2] = {{ 1, 1 }, { 0, 0 }, { 0, 1 }};  // possible throw positions
int loopCount = 0;      // delays printout to 1 out of 100 loops 
int throwDelay = 0;     // delays throwing turnout 1 out of 1000 loops

// SENSOR VARIABLES
int EOT_count = 0; // track if red LED lit
int selPin[] = { 5, 6, 9, 10 }; // A, B, C select pins on 4051
int SENS_CIO[] = { 2, 16 }; 
int CdSVal[] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // store last CdS readings
int EOT_varbs[4][4] = {{ 1, 300, 450, FORWARD },   /* east spur */
                       { 1, 300, 400, BACKWARD },  /* middle */
                       { 0, 240, 180, BACKWARD },  /* shed */
                       { 1, 300, 400, BACKWARD }}; /* chipper */                    

// DISPLAY VARIABLES
//int LED_CIO[] = { 16, 17 };
int LEDVal[] = { 0, 1, 0, 0, 0, 0, 0, 0 }; // leds to light
//int LEDValRev[] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // reverse LEDs
//int EOT_LEDS[] = {5, 1, 0, 2};
int EOT_LEDS[] = {6, 4, 5, 3};
int frontRedLED = 13;
int shedLight = 2;
boolean day = true;

int inByte;
byte outByte;
#include "train_funcs.h"

void setup()
{
  Serial.begin( 9600 ); // for printing diagnostics

  // setup SELECT PINS
  for( int pin = 0; pin <= 3; pin++ ){ // setup A,B,C,D select pins
    pinMode( selPin[pin], OUTPUT );
  }  

  // setup LED / SENS CIO pins
  //pinMode( LED_CIO[0], OUTPUT );
  //digitalWrite( LED_CIO[0], LOW );
  //pinMode( LED_CIO[1], OUTPUT );
  //digitalWrite( LED_CIO[1], LOW );
  pinMode( SENS_CIO[1], INPUT );
  digitalWrite( SENS_CIO[1], HIGH ); //engage pull-up
  pinMode( frontRedLED, OUTPUT );
  digitalWrite( frontRedLED, HIGH );
  pinMode( shedLight, OUTPUT );
  digitalWrite( shedLight, HIGH );
  
  // intialize 
  pinMode( throwPin[0], OUTPUT );  // intialize throwout control
  pinMode( throwPin[1], OUTPUT );
  digitalWrite( throwPin[0], LOW );
  digitalWrite( throwPin[1], LOW ); // should leave 2nd spur setup
  // nunchuck_setpowerpins();  // setup Chuk pins (if A2,A3 = gnd/pwr)
  nunchuck_init(); // send the initilization handshake
  readChuk();     // throw away first reading (not sure why needed)
  readChuk(); 
  joyxMid = joyx;  // assumes joystick centered on powerup
  if ( debug > 0 ){      // print intial readings
    Serial.print("\nWiiChuk Train Controller Ver.5\n");
    Serial.print("  speedLimit = "); Serial.println(speedLimit);
    Serial.print("  stallSpeed = "); Serial.println(stallSpeed);
    Serial.print("  rollDrag = "); Serial.println(rollDrag);
    Serial.print("  brakeDrag = "); Serial.println(brakeDrag);      
    Serial.print("\nCHUK reads:  accx: "); Serial.print((byte)accx,DEC);
    Serial.print("  joyx: "); Serial.print((byte)joyx,DEC);
    Serial.print("  cbut: "); Serial.print((byte)cbut,DEC);
    Serial.print("  zbut: "); Serial.println((byte)zbut,DEC);
    Serial.println("\nWaiting for joystick movement . . .");
    /*
    do{ // wait for joystick movement
      readChuk();
      MUX_LOOP();
    } while ( abs( joyx - joyxMid ) < deadBand );
    */
  }
  motor.setSpeed(0); 
  motor.run(trainDir);
  MUX_LOOP(); // get readings
  if (CdSVal[3] > 400 && CdSVal[4] > 400) { // lights off
    sensors = false;
    Serial.println("TOO DARK FOR SENSORS");
  }

  Serial.println("spur\twarn\ttrip");                       
  for(int spur = 0; spur < 4; spur ++){
    if (spur != 2){ // set CdS Vals
      EOT_varbs[spur][1] = CdSVal[spur + 3] + 50; // set EOT Warn
      EOT_varbs[spur][2] = CdSVal[spur + 3] + 150; // set EOT Trip
    }
    else { // set IR vals
      EOT_varbs[spur][1] = CdSVal[spur + 3] - 25; // set EOT Warn
      EOT_varbs[spur][2] = CdSVal[spur + 3] - 75; // set EOT Trip    
    }
    Serial.print(spur); Serial.print("\t");
    Serial.print(EOT_varbs[spur][1]); Serial.print("\t");
    Serial.println(EOT_varbs[spur][2]);
  }

  delay(1000);
  digitalWrite(frontRedLED, LOW);
  Serial.flush(); // clear buffers
}


void loop()
{
  if (Serial.available() > 0) {
    // read the incoming byte:
    inByte = Serial.read();

    if (inByte == 'a') { // button push on sound board
      if (debug > 0) {Serial.println("receive 'a' signal");}
      day = true;
      cruise = true;
      sensors = true;
      speedSet = cruiseSpeed;
      digitalWrite( shedLight, HIGH );
      delay(1000);
    }
    if (inByte == 'b') { // time to rest
      if (debug > 0) {Serial.println("received 'b' signal");}
      day = false;
      cruise = false;
      speedSet = 0;
      digitalWrite( shedLight, LOW );
      sensors = false;
      delay(1000);
    }
  }

  MUX_LOOP();
  readChuk();
  if ( joyx == 255 ) {// reset the connection
    nunchuck_init();
    if (debug > 0) {Serial.println( "CHUK RESET!" );}
    delay( 500 ); // just because
    readChuk(); // get new readings
    readChuk(); // discard first set
    joyxMid = joyx; // assum at midpoint on connection
  }
  
  // move the turnouts?
  if ( throwFlag == 1 && accx!= 0 ){ // chuk plugged in?
    //if ( joyy < 45 || joyy > 210 ){ // switch throws
    if (cbut) {
      
      //if ( joyy > 210 ) { // up one spur
        newSpur = spur + 1;
        if ( newSpur > 2 ) {
          newSpur = 0; 
        }
      //}
      //if ( joyy < 45 ) { // down one spur
      //  newSpur = spur - 1;
      //  if ( newSpur < 0 ) {
      //    newSpur = 2;
      //  }
      //}
      throwClear = true;
      if ( throwPos[spur][0] != throwPos[newSpur][0] ) { // need to move TO
        if ( CdSVal[1] > 500 || CdSVal[2] > 500){ // throw covered
          throwClear = false; // dont move T0 if T1 covered!
        }
      }
      if ( throwPos[spur][1] != throwPos[newSpur][1] ) { // need to move T1
        if ( CdSVal[2] > 500 ){ // throw covered
          throwClear = false;
        }
      }
      
      if ( throwClear || !sensors){ // ok to switch
        spur = newSpur;
        digitalWrite( throwPin[0], throwPos[spur][0] );
        if ( spur > 0 ) { // only move second throwout if spur = 1 or 2
          //delay(250); // short delay for power
          digitalWrite( throwPin[1], throwPos[spur][1] ); 
        }
        throwFlag = 0;  // disable throw move
        throwDelay = 0; // reset delay

        //TURNOUT LEDS
        if ( spur == 0 ){
          LEDVal[0] = 1;
          LEDVal[1] = 0;
          LEDVal[2] = 0;
        }
        if ( spur == 1 ){
          LEDVal[0] = 0;
          LEDVal[1] = 1;
          LEDVal[2] = 0;
        }
        if ( spur == 2 ){
          LEDVal[0] = 0;
          LEDVal[1] = 0;
          LEDVal[2] = 1;
        }        

        //Serial.print("Spur = "); Serial.print( spur );
        //Serial.print("   T0 = "); Serial.print( throwPos[spur][0] );
        //Serial.print("   T1 = "); Serial.println( throwPos[spur][1] );
      }
      else {
        if (debug > 0) {Serial.println("ON THROWS"); }
      }
    }
  }

  // throttle stuff
  if ( accx != 0 ){ // Chuk plugged in
    if ( zbut ) { // toggle cruise mode; re-added by Connor Ashcroft
      /*drag = brakeDrag; // increase drag
      */
       if (cruise){
         cruise=false;
       }else{
         cruise=true;
         speedSet=curSpeed;
       }
    } 
    else{ drag = rollDrag; } // normal momentum
    
    //if ( cbut ) { // turn on cruise control 
    //  cruise = true;
    //  speedSet = curSpeed;
    //} 
    
    throttle = joyx - joyxMid; // desired throttle
  
    if ( throttle < -deadBand && trainDir == FORWARD){ // slow down
      speedSet = 0;
      //cruise = false;  // turn off cruise control
      if (!day){
        day = true;
        digitalWrite( shedLight, HIGH );
        Serial.println('a'); // send signal to other board
      }
      drag = brakeDrag;
      if ( curSpeed < deadBand ){ // switch direction
        curSpeed = 0;
        trainDir = BACKWARD;
        motor.run( trainDir );
        if (debug > 0) {Serial.println("switch direction backward");}
      }
    }
    else if ( throttle > deadBand && trainDir == BACKWARD ){ // slow down
      speedSet = 0;
      //cruise = false;  // turn off cruise control
      drag = brakeDrag;
      if (!day){
        day = true;
        digitalWrite( shedLight, HIGH );
        Serial.println('a'); // send signal to other board
      }
      if ( curSpeed < deadBand ){ // switch direction
        curSpeed = 0;
        trainDir = FORWARD;
        motor.run( trainDir );
        if (debug > 0) {Serial.println( "switch direction forward" );}
      }
    }
    else {
      if ( !cruise ){
        if ( abs( throttle ) < deadBand ){
          speedSet = 0;
        }
        else {
          speedSet = abs( throttle * speedLimit / 100 );
        }
      }
    }
  }
    diff = ( speedSet - curSpeed ) / drag;  // determin change

    if ( curSpeed < stallSpeed ){ // handle slow speeds
      if ( curSpeed >= speedSet ){ // slowing, jump to zero
        curSpeed = 0;
      }
      else {
        curSpeed = stallSpeed;  // speedup, jump to stallSpeed
      }
    }
    else{ // implement drag
      //if (!cruise || diff > 0){ 
         curSpeed = curSpeed + diff;
      //}
    }

    // AUTO REVERSE CODE
    if (sensors){
      EOT_count = 0; // track if any EOT_trips
      for( int spur = 0; spur < 4; spur++ ){ // check each spur starting at 3
        if ( spur != 2 ){
          if ( CdSVal[spur + 3] > EOT_varbs[spur][1] ) { // EOT Warn
            LEDVal[EOT_LEDS[spur]] = 1; // turn on LED
            //Serial.print(spur);
            if ( trainDir != EOT_varbs[spur][3] ){ // heading off track
              if ( !EOT_warn ){
                EOT_warn = true; // set flag
                memSpeed = curSpeed;  // remember for reversal
                speedSet = warnSpeed;
                 cruise=false;
                if (debug > 0) {
                  Serial.print("EOT Warn on spur "); 
                  Serial.println(spur);
                }
              } 
              drag = brakeDrag; // slow down!
            }
            else { 
              EOT_warn = false;
              drag = rollDrag;
            } 
          }
          else {  
            LEDVal[EOT_LEDS[spur]] = 0; // turn off LED
          }
          if ( CdSVal[spur + 3] > EOT_varbs[spur][2] ){ // EOT Trip
            //digitalWrite(frontRedLED, HIGH);
            EOT_count = EOT_count + 1;
            if ( trainDir != EOT_varbs[spur][3] ){ // heading off track
              if ( !EOT_trip ){
                EOT_trip = true; // set flag
                trainDir = EOT_varbs[spur][3];
                curSpeed = 0;
                drag = rollDrag; // reset brakes
                speedSet = memSpeed;
                motor.run(trainDir);
                if (debug > 0) {
                  Serial.print("EOT TRIP on spur"); 
                  Serial.println(spur); 
                }
              }
            }     
          }
          else { 
            EOT_trip = false; 
            //digitalWrite(frontRedLED, LOW);
          }
        }
        else { // just for spur 2 (shed)
          if (CdSVal[spur + 3] < EOT_varbs[spur][1]) { // EOT Warn
            if (trainDir != EOT_varbs[spur][3]){ // heading off track
              if (!EOT_warn){
                EOT_warn = true; // set flag
                LEDVal[EOT_LEDS[spur]] = 1; // turn on LED
                memSpeed = curSpeed;  // remember for reversal
                speedSet = warnSpeed;
                 cruise=false;
                LEDVal[EOT_LEDS[spur]] = 1; // turn off LED 
                if (debug > 0) {
                  Serial.print("EOT Warn on spur "); 
                  Serial.println(spur);
                }
              } 
              drag = brakeDrag; // slow down!
            }
            else { 
              EOT_warn = false; 
              drag = rollDrag;
            } 
          }
          else{
            LEDVal[EOT_LEDS[spur]] = 0; // turn off LED 
          }  
          if ( CdSVal[spur + 3] < EOT_varbs[spur][2] ){ // EOT Trip
            EOT_count = EOT_count + 1;            
            if ( trainDir != EOT_varbs[spur][3] ){ // heading off track
              if ( !EOT_trip ){
                EOT_trip = true; // set flag
                LEDVal[EOT_LEDS[spur]] = 1; // turn on LED
                trainDir = EOT_varbs[spur][3];
                curSpeed = 0;
                drag = rollDrag; // reset brakes
                speedSet = memSpeed;
                motor.run(trainDir);
                if (debug > 0) {
                  Serial.print("EOT TRIP on spur"); 
                  Serial.println(spur); 
                }
              }
            }     
          }
          else { 
            EOT_trip = false; 
            //digitalWrite(frontRedLED, LOW);
          }
        }
      }
      if (EOT_count > 0){
        digitalWrite(frontRedLED, HIGH);
        LEDVal[7] = 0;
      }
      else {
        digitalWrite(frontRedLED, LOW);
        LEDVal[7] = 0;
      }
    } // end if (sensors)
    
    if (cruise) {  // Reverse direction if speed = 0
      if (curSpeed == 0) { // reverse direction
       if (trainDir == 1) {
         trainDir = 2;
       }
       else {
         trainDir = 1;
       }
       curSpeed = 5;
       motor.run(trainDir);
       speedSet = memSpeed;
      } 
    }
    
    
/*
    // throttle LEDS
    float speedPct = curSpeed / speedLimit;  // % of full speed
    if ( trainDir == 1 ){
      if ( speedPct > 0 ){ LEDVal[7] = 1; }
      else { LEDVal[7] = 0; } 
      if ( speedPct > 0.3 ){ LEDValRev[7] = 1; }
      else { LEDValRev[7] = 0; } 
      if ( speedPct > 0.6){ LEDVal[6] = 1; }    
      else { LEDVal[6] = 0; } 
      if ( speedPct > 0.8){ LEDValRev[6] = 1; }
      else { LEDValRev[6] = 0; }
    }
    else{
      if ( speedPct > 0){LEDValRev[6] = 1; }
      else { LEDValRev[6] = 0; } 
      if ( speedPct > 0.3 ){ LEDVal[6] = 1; }
      else { LEDVal[6] = 0; } 
      if ( speedPct > 0.6 ){ LEDValRev[7] = 1; }    
      else { LEDValRev[7] = 0;} 
      if ( speedPct > 0.8 ){ LEDVal[7] = 1; }
      else { LEDVal[7] = 0; }      
    }

*/

    motor.setSpeed( abs( curSpeed ));
    //delay(100); // don't need rapid change
    if ( debug == 1 && loopCount == 0 ){ // show what you want to see
      //Serial.print("accx: "); Serial.print((byte)accx,DEC);
      //Serial.print("  joyx: "); Serial.print((byte)joyx,DEC);
      Serial.print("  joyy: "); Serial.print((byte)joyy,DEC);
      //Serial.print("  zbut: "); Serial.print((byte)zbut,DEC);
      Serial.print("EOT_warn: "); Serial.print( EOT_warn );
      Serial.print("  EOT_trip: "); Serial.print( EOT_trip );
      //Serial.print("  drag: "); Serial.print( drag );      
      Serial.print("  trainDir: "); Serial.print(trainDir);
      //Serial.print("  spur: "); Serial.print(spur); 
      //Serial.print("  throttle: "); Serial.print(throttle);    
      Serial.print("  speedSet: "); Serial.print( speedSet );
      Serial.print("  curSpeed: "); Serial.print( curSpeed );
      //Serial.print("  speedPct: "); Serial.print(speedPct);      
      Serial.print("  memSpeed: "); Serial.print( memSpeed );
      //Serial.print("  diff: "); Serial.print(diff);
      //Serial.print("  cruise: "); Serial.println(cruise);
      Serial.println();
    }
    sendOut(); // serial message for LEDs
    loopCount ++;
    if ( loopCount > 100 ) {
      loopCount = 0;
      throwDelay ++;
      if ( throwDelay > 1 ){ 
        throwFlag = 1;  // enable throw move
        throwDelay = 0;
      }
    }
  //}
  /*
  else{
    Serial.println("WHERE IS CHUK?");
    delay( 1000 ); // wait before checking again
  }*/
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

