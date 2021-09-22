#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial GYNSerial(5, 6);
TinyGPSPlus    gps;

static const int MAX_LAT_LONG = 12;
static const int MAX_DATA     = 15;
static char      latitude [MAX_LAT_LONG];
static char      longitude[MAX_LAT_LONG];
static char      data     [MAX_DATA];

void setup() {

  Serial.begin(9600);
  GYNSerial.begin(9600);

  strncpy(latitude, "Indefinido",  MAX_LAT_LONG);
  strncpy(longitude,"Indefinido",  MAX_LAT_LONG);
  strncpy(data,"Indefinido", MAX_DATA);

}

void loop() {

  while(GYNSerial.available()){
    gps.encode(GYNSerial.read());
  }

  if(gps.location.isUpdated()){
    
    dtostrf(gps.location.lat(),9,7,latitude);
    dtostrf(gps.location.lng(),9,7,longitude);

    sprintf(data,"%lu%02d%02d%02d", gps.date.value(),(uint32_t*)(gps.time.hour()-3),(uint32_t*)gps.time.minute(),(uint32_t*)gps.time.second());
  
    Serial.println("[*] Localizacao");
    Serial.print("\tData -> ");
    Serial.println(data);
    Serial.print("\tLatitude -> ");
    Serial.println(latitude);
    Serial.print("\tLongitude -> ");
    Serial.println(longitude);
    
  }

  delay(200);
  
}
