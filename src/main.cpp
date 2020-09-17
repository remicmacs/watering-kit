#include <Arduino.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <dht.h>

dht DHT;
#define DHT11_PIN    9
#define MOISTURE_PIN A2
#define VARISTOR_IN A1
#define DEBUG_LED 12

#define SLEEP_CYCLES 180 * 15

int airHumidity;
int airTemperature;
int soilHumidity;
int varistor;
// Safe value to be ensure no false positives
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
  // disable all interrupts
  cli();
  // reset the WDT timer
  wdt_reset();
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

#ifdef LEDDEBUG
void wakeBlink() {
  int nb = 5;
  while(nb-- != 0) {
    digitalWrite(DEBUG_LED, HIGH);
    delay(50);
    digitalWrite(DEBUG_LED, LOW);
    delay(50);
  }
  delay(500);
  nb = 5;
  while(nb-- != 0) {
    digitalWrite(DEBUG_LED, HIGH);
    delay(50);
    digitalWrite(DEBUG_LED, LOW);
    delay(50);
  }
}

void interruptBlink() {
  int n = 3;
  while(n--) {
    digitalWrite(DEBUG_LED, HIGH);
    delay(300);
    digitalWrite(DEBUG_LED, LOW);
    delay(300);
  }
}

void setupBlink() {
  int nb = 10;
  while(nb-- != 0) {
    digitalWrite(DEBUG_LED, HIGH);
    delay(50);
    digitalWrite(DEBUG_LED, LOW);
    delay(50);
  }
}
#endif

void pollData(){
  //Read Data
  DHT.read11(DHT11_PIN);
  airHumidity = DHT.humidity;
  airTemperature = DHT.temperature;
  soilHumidity = analogRead(MOISTURE_PIN);
  varistor = analogRead(VARISTOR_IN);
}

#ifndef LEDDEBUG
void startPump() {
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
}

void stopPump() {
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
}

/**
 * Initialize the pins driving the pump and turn the pump off
 */
void setupPump() {
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  stopPump();
}

void wateringRoutine() {
  stopPump();
  pollData();
  while(soilHumidity <=varistor) {
    startPump();
    pollData();
  }
  stopPump();
}
#endif

void setup() {

#ifdef LEDDEBUG
  // Just to see the setup step
  setupBlink();
  //initialize LED digital pin as an output.
  pinMode(DEBUG_LED, OUTPUT);
#endif

#ifndef LEDDEBUG
  setupPump();
#endif
  /* Disable the watchdog and wait for more than 2 seconds */
  /* Done so that the Arduino doesn't keep resetting infinitely in case of wrong configuration */
  wdt_disable();
  delay(3000);
}

void loop() {
  // Signal interrupt
  if (9 == interrupted) {
    sleepCycles--;
#ifdef LEDDEBUG
    interruptBlink();
#endif
    interrupted = 0;

    if (sleepCycles <= 0) {
#ifdef LEDDEBUG
      wakeBlink();
#endif
#ifndef LEDDEBUG
      // Watering cycle
      wateringRoutine();
#endif
      sleepCycles = SLEEP_CYCLES;
    }
  }
  timedSleep();
}