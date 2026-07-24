# Badge Firmware

C/C++ firmware for the ESP32-based smart badge — controls WS2812 LED effects, reads capacitive touch input, and communicates with the browser controller over serial.

## Status
🚧 In Progress

## Planned Features
- 3 light scenes: steady glow, sine wave, pulse
- Touch input cycles between scenes
- JSON command protocol over serial

## Tools
- PlatformIO
- FastLED or Adafruit_NeoPixel

## Related repos
- badge-pcb — hardware this runs on
- badge-web — browser controller that sends commands to this firmware
