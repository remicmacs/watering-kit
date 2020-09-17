# Automated watering system

Based on the [EcoDuino kit](https://wiki.dfrobot.com/EcoDuino_-_An_Auto_Plant_Kit_SKU__KIT0003).

## How to build

This project uses [platformio](https://platformio.org/).

Clone the repo, plug in the development board and

```bash
pio run -v -e prod --target upload
```

go brrrr !

### Upload problems

When the microcontroller goes to sleep (`SLEEP_MODE_PWR_DOWN`), it's possible that the next upload will fail.

To solve :

- Find your upload port (`pio device list` in a loop shall do the trick).
- Add `--upload-port <my upload port path>` to the command
- When platformio displays "Waiting for upload port" or something, press the reset button

When the reset button is pressed, a pulsing signal is sent on the digital pin 13. Just plug in a LED and you shall see the light !

## Environments

Available environments :

* leddebug
  * debug sleep routines with LED indicators. Useful because sleep kills the serial
  * the output pin to drive the pump are disabled
  * watering routine is not compiled (pump needs 5 to 12V to operate, not that easy to debug)
* prod :
  * no LED output pin is enabled
  * watering routine is activated