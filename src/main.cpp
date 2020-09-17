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
int counter = 0;


#ifndef DEBUGLED
#define DEBUGLED 12
#endif

volatile int interrupted = 9;

/**
 * Interrupt configured on watchdog vector
 */
ISR(WDT_vect) {
  cli();
  wdt_reset();
  interrupted = 9;
}

void watchdogSetup(void){
  cli(); // disable all interrupts
  wdt_reset(); // reset the WDT timer
  /**
   * WDTCSR configuration:
   * WDIE  = 1: Interrupt Enable
   * WDE   = 1 :Reset Enable
   */
  // Enter Watchdog Configuration mode:
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // Set Watchdog settings: 8s
  WDTCSR = (1<<WDIE) | (1<<WDE) | (1<<WDP3)  | (0<<WDP2) | (0<<WDP1)  | (1<<WDP0);
  sei();
}

/**
 * Sends the microcontroller to sleep
 */
void deepSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli();
  sleep_enable();
  sei();
  sleep_cpu();
  sleep_disable();
  sei();
}

void timedSleep() {
  watchdogSetup();
  deepSleep();
}

void pollBlink() {
  digitalWrite(DEBUGLED, HIGH);
  delay(500);
  digitalWrite(DEBUGLED, LOW);
  delay(300);
  digitalWrite(DEBUGLED, HIGH);
  delay(500);
  digitalWrite(DEBUGLED, LOW);
  delay(300);
}

void interruptBlink() {
  int n = 3;
  while(n--) {
    digitalWrite(DEBUGLED, HIGH);
    delay(300);
    digitalWrite(DEBUGLED, LOW);
    delay(300);
  }
}

void setupBlink() {
  int nb = 10;
  while(nb-- != 0) {
    digitalWrite(DEBUGLED, HIGH);
    delay(50);
    digitalWrite(DEBUGLED, LOW);
    delay(50);
  }
}

void pollData(){
  pollBlink();
  int chk;
  chk = DHT.read11(DHT11_PIN);   //Read Data
  switch (chk){
    case DHTLIB_OK:
      // Serial.print("OK,\t");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      // Serial.print("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      // Serial.print("Time out error,\t");
      break;
    default:
      // Serial.print("Unknown error,\t");
      break;
  }
  airHumidity = DHT.humidity;
  airTemperature = DHT.temperature;
  soilHumidity = analogRead(MOISTURE_PIN);
  varistor = analogRead(VARISTOR_IN);

  // Serial.print(F("airHumidity:"));
  // Serial.print(airHumidity);
  // Serial.print(F(",\t"));
  // Serial.print(F("airTemperature:"));
  // Serial.print(airTemperature);
  // Serial.print(F(",\t"));50
  // Serial.print(F("Soil Humidity : "));
  // Serial.print(soilHumidity);
  // Serial.print(F(",\t"));
  // Serial.print(F("varistor : "));
  // Serial.print(varistor);
  // Serial.print(F("\n"));
  // delay(1000);
}

void setup() {
  // Just to see the setup step
  setupBlink();
  // initialize LED digital pin as an output.
  pinMode(DEBUGLED, OUTPUT);

  /* Disable the watchdog and wait for more than 2 seconds */
  /* Done so that the Arduino doesn't keep resetting infinitely in case of wrong configuration */
  wdt_disable();
  delay(3000);
}

void loop() {
  // Signal interrupt
  if (9 == interrupted) {
    interruptBlink();
    interrupted = 0;
  }
  pollData();
  timedSleep();
}