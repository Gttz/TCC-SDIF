void setup() {
  
  analogReference(INTERNAL1V1);
  
  Serial.begin(9600);
  
  pinMode(A0, INPUT);
  
}

void loop() {
  
  double t = analogRead(A0)/9.31;

  Serial.print("[*] Temperatura -> ");
  Serial.println(t);

  delay(5000);

}
