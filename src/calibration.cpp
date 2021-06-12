#include <Arduino.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <dht.h>

dht DHT;
#define DHT11_PIN    9
#define MOISTURE_PIN A2
#define VARISTOR_IN A1
#define DHT_RESOLUTION_TIME 1139

int airHumidity;
int airTemperature;
int soilHumidity;
int varistor;

void pollData(){
  int chk;
  chk = DHT.read11(DHT11_PIN);
  switch (chk){
    case DHTLIB_OK:
      Serial.print("OK,             \t");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum error ,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Timeout error  ,\t");
      break;
    case DHTLIB_ERROR_CONNECT:
      Serial.print("Connect error  ,\t");
      break;
    case DHTLIB_ERROR_ACK_L:
      Serial.print("Low ACK error  ,\t");
      break;
    case DHTLIB_ERROR_ACK_H:
      Serial.print("High ACK error ,\t");
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
}

void setup() {
  Serial.begin(921600);

}

void loop() {
  // use active polling rather than delay() function
  // DHT_RESOLUTION_TIME is the minimum timeframe acceptable
  // in order to avoid DHTLIB_ERROR_CONNECT
  // Value has been found empirically with a binary search
  // Lower _will_ raise connect errors and then the resolution
  // would be slower anyway
  static uint32_t lastTime = 0;
  if (millis() - lastTime >= DHT_RESOLUTION_TIME) {
    lastTime = millis();
    pollData();
  }
}
