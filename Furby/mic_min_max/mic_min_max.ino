/**
 * Measures the voltage on an analog pin over a ~1s period of time
 * and sends the Min, Max and Diff (Spread) values over Serial.
 *
 * Author: Dimitar Kovachev, http://lowvoltage.wordpress.com/
 *
 * Released under the Creative Commons Attribution Share-Alike 3.0 license
 * http://creativecommons.org/licenses/by-sa/3.0/
 */
const int analogPin = 0;

void setup() {
  Serial.begin(9600); 
}

void loop() {
  int mn = 1024;     // mn only decreases
  int mx = 0;        // mx only increases

  // Perform 10000 reads. Update mn and mx for each one.
  for (int i = 0; i < 10000; ++i) {
    int val = analogRead(analogPin);
    mn = min(mn, val);
    mx = max(mx, val);
  }

  // Send min, max and delta over Serial
  Serial.print("m=");
  Serial.print(mn);  
  Serial.print(" M=");
  Serial.print(mx);  
  Serial.print(" D=");
  Serial.print(mx-mn);  
  Serial.println();  
}
