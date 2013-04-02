void sendOut(){
  byte oldOutByte = outByte;
  outByte = 0;
  for (int x = 0; x < 7; x++){
    outByte = outByte | (LEDVal[x] << x);  
  }
  outByte = outByte + 128;
  //Serial.print("outByte="); Serial.println(outByte, BIN);
  if (outByte != oldOutByte){
    //Serial.print(outByte); // send it out
    Serial.write(outByte); // 1.0 CODE
    if (debug > 0) {Serial.println(outByte, BIN);}
  }
}

void MUX_LOOP() {

  for (int bank = 1; bank < 7; bank++) {
    digitalWrite(selPin[3], HIGH);  // send CD4067 to 8+
    for(int pin = 2; pin >= 0; pin--) { // parse out select pin bits
      int signal = (bank >> pin) & 1;  // shift  & bitwise compare
      digitalWrite(selPin[pin], signal);
      //if (debug == 2){ Serial.print(signal); }
    }
    /*
    if (LEDVal[bank]){ // turn it on
      digitalWrite(LED_CIO[0], HIGH);
      delayMicroseconds(500);
      //digitalWrite(LED_CIO[0], LOW);
    }*/
    if (bank == 5) {delay(10);} // delay just a bit for the transisor to stabilize
    CdSVal[bank] = analogRead(SENS_CIO[0]); 
    if (debug == 2 && loopCount == 0) {
      Serial.print(bank);  Serial.print(" - ");
      Serial.print(CdSVal[bank]); Serial.print("  ");
    }
    /*
    if (LEDVal[bank]){ // turn it off
      digitalWrite(LED_CIO[0], LOW);
    }
    if (LEDValRev[bank]){ // reverse color
      digitalWrite(LED_CIO[1], HIGH);
      delayMicroseconds(500);
      digitalWrite(LED_CIO[1], LOW);        
    }*/
  }  
  if (debug == 2 && loopCount == 0){
    Serial.println();
  }
}

/*
void readCdS() {

  //pinMode(commonPin[0], INPUT);  // setup CIO input
  //digitalWrite(commonPin[0], HIGH);  // turn CIO pull-up on
  for (int bank = 1; bank < 7; bank++) {
    for(int pin = 2; pin >= 0; pin--) { // parse out select pin bits
      int signal = (bank >> pin) & 1;  // shift  & bitwise compare
      digitalWrite(selPin[pin], signal);
      //if (debug == 2){ Serial.print(signal); }
    }
    if (bank == 5) {delay(1);} // delay just a bit for the transisor to stabilize
    CdSVal[bank] = analogRead(SENS_CIO[0]); 
    if (debug == 2 && loopCount == 0) {
      Serial.print(bank);  Serial.print(" - ");
      Serial.print(CdSVal[bank]); Serial.print("  ");
    }
  }  
  if (debug == 2 && loopCount == 0){
    Serial.println();
  }
  //digitalWrite(commonPin[0], LOW);  // turn CIO pull-up off
}
*/

void readChuk(){ // check Chuk readings
  //nunchuck_get_data(); // from nunchuck_funcs.h
  nunchuk.update();
  //delay(1); // not sure why needed, but keeps first data set clean
  //accx  = nunchuck_accelx(); // 70 - 125 - 182
  accx = nunchuk.accelX;
  //accy  = nunchuck_accely(); // ranges from approx 65 - 112 - 173
  //accz  = nunchuck_accelz(); // ranges from approx ??  ??
  joyx = nunchuk.analogX;  //34 - 137 - 234
  joyy = nunchuk.analogY;  //34 - 129 - 217
  zbut = nunchuk.zButton;
  cbut = nunchuk.cButton; 
  
}

/*
void flashLEDs() {

  
  //for (int pin = 0; pin < 2; pin++){ // setup CIO pins
  //  pinMode(commonPin[pin], OUTPUT);
  //  digitalWrite(commonPin[pin], LOW);
  //}
  for (int bank = 8; bank < 16; bank++) {
    if (LEDVal[bank - 8] || LEDValRev[bank - 8]){ // need to light one up
      for(int pin = 3; pin >= 0; pin--) { // parse out select pin bits
        int signal = (bank >> pin) & 1;  // shift  & bitwise compare
        digitalWrite(selPin[pin], signal);
        //Serial.print(signal);
      }
      if (LEDVal[bank - 8]){
        digitalWrite(LED_CIO[0], HIGH);
        delayMicroseconds(500);
        digitalWrite(LED_CIO[0], LOW);
      }
      if (LEDValRev[bank - 8]){
        digitalWrite(LED_CIO[1], HIGH);
        delayMicroseconds(500);
        digitalWrite(LED_CIO[1], LOW);        
      }
    }
  }  
}
*/

void reverseDir() {
  if (trainDir == FORWARD){
    trainDir = BACKWARD;
  }
  else {
    trainDir = FORWARD;
  }
}
