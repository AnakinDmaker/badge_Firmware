#include <ArduinoJson.h>
#include <Preferences.h>
#include <FastLED.h>

Preferences preferences;

#define LED_PIN 4
#define NUM_LEDS 16
CRGB leds[NUM_LEDS];

int currentScene = 0;
int brightness = 255; // Default brightness
bool wasTouched = false;
const char* sceneNames[] = {"Steady", "Wave", "Pulse"};

unsigned long lastUpdate = 0;
const unsigned long updateInterval = 50;
unsigned long loopCount = 0;
unsigned long updateCount = 0;
unsigned long lastPrint = 0;

const unsigned long debounceDelay = 30;
unsigned long lastChangeTime = 0;
bool stableTouchState = false;
bool lastRawState = false;


void setup() {
  Serial.begin(115200);
  preferences.begin("badge", false);
  currentScene = preferences.getInt("scene", 0);
  brightness = preferences.getInt("brightness", 255);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
}

void saveSettings() {
  preferences.putInt("scene", currentScene);
  preferences.putInt("brightness", brightness);
}

void sendStatus() {
  JsonDocument response;
  response["scene"] = sceneNames[currentScene];
  response["brightness"] = brightness;
  serializeJson(response, Serial);
  Serial.println();
}

void handleTouch() {
  int touchValue = touchRead(4);
  bool rawState = touchValue < 30;

  if (rawState != lastRawState) {
    lastChangeTime = millis();
  }
  lastRawState = rawState;

  if ((millis() - lastChangeTime) > debounceDelay) {
    if (rawState != stableTouchState) {
      stableTouchState = rawState;
      if (stableTouchState) {
        currentScene = (currentScene + 1) % 3;
        Serial.print("Scene changed to: ");
        Serial.println(sceneNames[currentScene]);
        saveSettings();
      }
    }
  }
}

void runSteadyScene() {
  fill_solid(leds, NUM_LEDS, CRGB(255, 120, 0));
  FastLED.show();
}

void runWaveScene() {
  static uint8_t hueOffset = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t wave = sin8(hueOffset + (i * 255 / NUM_LEDS));
    leds[i] = CRGB(scale8(255, wave), scale8(120, wave), 0);
  }
  hueOffset += 3;
  FastLED.show();
}

void runPulseScene() {
  static bool on = false;
  fill_solid(leds, NUM_LEDS, on ? CRGB(255, 120, 0) : CRGB::Black);
  on = !on;
  FastLED.show();
}

void updateLEDs() {
  if (currentScene == 0) runSteadyScene();
  else if (currentScene == 1) runWaveScene();
  else if (currentScene == 2) runPulseScene();
}

void handleSerialCommand() {
  if (!Serial.available()) return;

  String line = Serial.readStringUntil('\n');
  line.trim();

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, line);

  if (error) {
    Serial.print("JSON error: ");
    Serial.println(error.c_str());
    return;
  }

  if (doc["effect"].is<const char*>()) {
    const char* effect = doc["effect"];
    for (int i = 0; i < 3; i++) {
      if (strcmp(effect, sceneNames[i]) == 0) {
        currentScene = i;
        Serial.print("Scene set via command: ");
        Serial.println(sceneNames[currentScene]);
      }
    }
  }

  if (doc["brightness"].is<int>()) {
    brightness = doc["brightness"];
    Serial.print("Brightness set to: ");
    Serial.println(brightness);
  }

  if (doc["cmd"].is<const char*>()) {
    const char* cmd = doc["cmd"];
    if (strcmp(cmd, "get_status") == 0) {
      sendStatus();
    }
  }
}

void loop() {
  handleTouch();
  handleSerialCommand();
  updateLEDs();
  
  loopCount++;

  unsigned long now = millis();

  if (now - lastUpdate >= updateInterval) {
    lastUpdate = now;
    updateLEDs();
    updateCount++;
  }

  if (now - lastPrint >= 1000) {
    lastPrint = now;
    Serial.print("Loop runs/sec: ");
    Serial.print(loopCount);
    Serial.print(" | Update runs/sec: ");
    Serial.println(updateCount);
    loopCount = 0;
    updateCount = 0;
  }
}

//  {"effect":"Wave"}
//  {"effect":"Steady"}
//  {"effect":"Pulse"}
//  {"brightness":100}
//  {"effect":"Wave","brightness":50}
//  {"cmd":"get_status"}