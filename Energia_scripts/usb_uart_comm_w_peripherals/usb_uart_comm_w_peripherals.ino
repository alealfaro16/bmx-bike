
char c = ' ';
void setup() {
  // initialize ports:
  Serial.begin(9600); //connected with USB
  delay(100);
  Serial1.begin(38400); // Rx = PB0; Tx = PB1
  delay(100);
  Serial.println("Initialization finished");
}

void loop() {

  if (Serial.available()) {
    // get the new byte:
    c = (char)Serial.read();
    //send it to peripheral
    Serial1.write(c); 
  }
  if (Serial1.available()) {
    // get the new byte:
    c = (char)Serial1.read();
    //send it to UI
    Serial.write(c); 
  } 
}
