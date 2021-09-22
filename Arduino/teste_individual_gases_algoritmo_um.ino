#include <MQUnifiedsensor.h>

#define placa "Arduino Uno"
#define resolucao_voltagem 5
#define pino A1
#define tipo "MQ-135"
#define adc_resolucao_bit 10
#define razao_ar_limpo 3.6

MQUnifiedsensor MQ135(placa, resolucao_voltagem, adc_resolucao_bit, pino, tipo);

void setup() {
  
  Serial.begin(9600);

  MQ135.setRegressionMethod(1);

  MQ135.init(); 
  MQ135.setRL(1.0);

  Serial.println("[*] Calibrando - pré-aquecido...");
  
  float calcR0 = 0;
  
  for(int i=0; i<10; i++){
    
    MQ135.update();
    calcR0 += MQ135.calibrate(razao_ar_limpo);
    
  }
  
  MQ135.setR0(calcR0/10);
  
  Serial.println("[*] Calibragem finalizada");
    
}

void loop() {
  
  MQ135.update();

  MQ135.setA(110.47);
  MQ135.setB(-2.862);
  
  float CO2 = MQ135.readSensor();

  Serial.print("[*] CO2 -> "); 
  Serial.println(CO2 + 400);
  
  delay(5000);
  
}
