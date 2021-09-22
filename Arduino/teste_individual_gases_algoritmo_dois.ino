float RZERO;
float PARA = 116.6020682;
float PARB = 2.769034857;
float RLOAD = 1.0;

void setup() {

  Serial.begin(9600);

  RZERO = (((1023./(float)analogRead(A0))*5.-1.)*RLOAD) * pow((419.13/PARA), (1./PARB));

}

void loop() {
  float ppm = PARA * pow(((((1023./(float)analogRead(A0))*5.-1.)*RLOAD)/RZERO), -PARB);

  Serial.print("[*] CO2 -> ");
  Serial.println(ppm);

  delay(5000);
}
