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
volatile int sleepCycles = 0;

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
  /**
   * Setting the watchdog pre-scaler value with VCC = 5.0V and 16mHZ
   * WDP3 WDP2 WDP1 WDP0 | Number of WDT | Typical Time-out at Oscillator Cycles
   * 0    0    0    0    |   2K cycles   | 16 ms
   * 0    0    0    1    |   4K cycles   | 32 ms
   * 0    0    1    0    |   8K cycles   | 64 ms
   * 0    0    1    1    |  16K cycles   | 0.125 s
   * 0    1    0    0    |  32K cycles   | 0.25 s
   * 0    1    0    1    |  64K cycles   | 0.5 s
   * 0    1    1    0    |  128K cycles  | 1.0 s
   * 0    1    1    1    |  256K cycles  | 2.0 s
   * 1    0    0    0    |  512K cycles  | 4.0 s
   * 1    0    0    1    | 1024K cycles  | 8.0 s
  */
  // Set Watchdog settings: 4s
  WDTCSR = (1<<WDIE) | (1<<WDE) | (1<<WDP3)  | (0<<WDP2) | (0<<WDP1)  | (0<<WDP0);
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
  int nb = 5;
  while(nb-- != 0) {
    digitalWrite(DEBUGLED, HIGH);
    delay(50);
    digitalWrite(DEBUGLED, LOW);
    delay(50);
  }
  delay(500);
  nb = 5;
  while(nb-- != 0) {
    digitalWrite(DEBUGLED, HIGH);
    delay(50);
    digitalWrite(DEBUGLED, LOW);
    delay(50);
  }
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
    sleepCycles--;
    interruptBlink();
    interrupted = 0;

    if (sleepCycles <= 0) {
      // Poll data only once the sleepCycles are exhausted
      pollData();
      // 15 * 4s = 1 min
      sleepCycles = 15;
    }
  }
  timedSleep();
}