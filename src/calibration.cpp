#include <Arduino.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <dht.h>

dht DHT;
#define DHT11_PIN    9
#define MOISTURE_PIN A2
#define VARISTOR_IN A1

int airHumidity;
int airTemperature;
int soilHumidity;
int varistor;

void pollData(){
  int chk;
  chk = DHT.read11(DHT11_PIN);
  switch (chk){
    case DHTLIB_OK:
      Serial.print("OK,            \t");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Time out error,\t");
      break;
    default:
      Serial.print("Unknown error, \t");
      break;
  }
  airHumidity = DHT.humidity;
  airTemperature = DHT.temperature;
  soilHumidity = analogRead(MOISTURE_PIN);
  varistor = analogRead(VARISTOR_IN);

  Serial.print(F("airHumidity:"));
  Serial.print(airHumidity);
  Serial.print(F(",\t"));
  Serial.print(F("airTemperature:"));
  Serial.print(airTemperature);
  Serial.print(F(",\t"));
  Serial.print(F("soilHumidity : "));
  Serial.print(soilHumidity);
  Serial.print(F(",\t"));
  Serial.print(F("varistor : "));
  Serial.print(varistor);
  Serial.print(F("\n"));
  delay(300);
}

void setup() {
  Serial.begin(921600);

}

void loop() {
  pollData();
}
