# Badge Firmware

C/C++ firmware for the ESP32-based smart badge — controls WS2812 LED effects, reads capacitive touch input, and communicates with the browser controller over serial.

## Status
✅ Core logic complete and tested — LED animation pending physical hardware

![Status](https://img.shields.io/badge/status-working-brightgreen)

## Working Features
- 3 light scenes: steady, sine wave, pulse (scene logic implemented)
- Touch input cycles between scenes (edge-detected, calibrated)
- Bidirectional JSON command protocol over serial (effect, brightness, get_status)
- Persistent settings via ESP32 Preferences — survives power loss
- Non-blocking main loop (millis-based timing)

## Tools
- PlatformIO
- ArduinoJson

## Related repos
- badge-pcb — hardware this runs on
- badge-web — browser controller that sends commands to this firmware

## Platform

![Platform](https://img.shields.io/badge/platform-ESP32--S3-orange)

## License

![License](https://img.shields.io/badge/license-MIT-blue)
