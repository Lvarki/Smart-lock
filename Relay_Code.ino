void setup() {
  pinMode(6, OUTPUT); // Set pin 6 as an output
}

void loop() {
  digitalWrite(6, HIGH); // Turn ON
  delay(3000); // Wait 3 seconds
  digitalWrite(6, LOW); // Turn OFF
  delay(3000); // Wait 3 seconds
}
