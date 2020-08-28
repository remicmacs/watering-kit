#include <Arduino.h>

#include <dht.h>
dht DHT;
#define DHT11_PIN    9
#define MOISTURE_PIN A2
#define POTAR_IN A1

int airHumidity;   //environment humidity
int airTemperature;  // environment temperature
int soilHumidity;
int potar;
int counter = 0;

#include <avr/wdt.h>


#ifndef MYLED
#define MYLED 12
#endif

volatile int interrupted = 9;
volatile int on = 0;

void idleSleep() {

  SMCR = (1<<SE) | (0<<SM2) | (0<<SM1) | (0<<SM0) ;
}

void pollData(){
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
  potar = analogRead(POTAR_IN);

  // Serial.print(F("airHumidity:"));
  // Serial.print(airHumidity);
  // Serial.print(F(",\t"));
  // Serial.print(F("airTemperature:"));
  // Serial.print(airTemperature);
  // Serial.print(F(",\t"));
  // Serial.print(F("Soil Humidity : "));
  // Serial.print(soilHumidity);
  // Serial.print(F(",\t"));
  // Serial.print(F("Potar : "));
  // Serial.print(potar);
  // Serial.print(F("\n"));
  // delay(1000);
}

void blink(int numberof) {
  while(numberof--) {
    digitalWrite(MYLED, HIGH);
    delay(100);
    digitalWrite(MYLED, LOW);
    delay(100);
  }
}

void setupBlink() {
  int nb = 10;
  while(nb-- != 0) {
    digitalWrite(MYLED, HIGH);
    delay(50);
    digitalWrite(MYLED, LOW);
    delay(50);
  }
}

void turnOn() {
  digitalWrite(MYLED, HIGH);
}

void turnOff() {
  digitalWrite(MYLED, LOW);
}

void toggle() {
  if (on) {
    turnOff();
    on = 0;
  } else {
    turnOn();
    on = 1;
  }
}

ISR(WDT_vect) {
  wdt_reset();
  cli();
  interrupted = 9;
}

void watchdogSetup(void){
  cli(); // disable all interrupts
  wdt_reset(); // reset the WDT timer
  /*   WDTCSR configuration:
    WDIE  = 1: Interrupt Enable
    WDE   = 1 :Reset Enable
  */
  // Enter Watchdog Configuration mode:
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // Set Watchdog settings:
  WDTCSR = (1<<WDIE) | (1<<WDE) | (1<<WDP3)  | (0<<WDP2) | (0<<WDP1)  | (1<<WDP0);
  sei();
}

void setup()
{
  // initialize LED digital pin as an output.
  // Serial.begin(115200);
  // Serial.println("SETUP");
  pinMode(MYLED, OUTPUT);
  setupBlink();

  /* Disable the watchdog and wait for more than 2 seconds */
  /* Done so that the Arduino doesn't keep resetting infinitely in case of wrong configuration */
  wdt_disable();
  delay(3000);
}

void loop()
{

  if (9 == interrupted) {
    // Serial.println("Interrupted");
    blink(3);
    // toggle();
    interrupted = 0;
    pollData();
    watchdogSetup();
    // idleSleep();
  }
}