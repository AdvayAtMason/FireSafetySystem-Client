#include <WiFi.h>
#include <HTTPClient.h>
#include "Arduino.h"
#include <Arduino_JSON.h>
#include "DFRobotDFPlayerMini.h"
#define FPSerial Serial1


DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

// Can be "inactive" or "active"
String alarmMode = "inactive";

// LED settings
int redBaseBrightness = 255;
int greenBaseBrightness = 255;
int blueBaseBrightness = 160;
float brightness = 0;
float brightnessFactor = 1;
float fadeAmount = 0.25;

// Speaker settings
float alarmVolume = 1;
int timer = 0;
int TIMER_INTERVAL = 8650;

String ledState = "off";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(27, OUTPUT);

  // Use access point
  // Serial.println("Connecting...");
  // WiFi.softAP("ESP32-Access-Point", "123456789");
  // Serial.println(WiFi.softAPIP());

  // Use WiFi
  char* ssid = "Nacho wifi";
  char* password = "Stacey30";
  Serial.println("Connecting...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());;

  // Setup DFPlayer
  FPSerial.begin(9600, SERIAL_8N1, /*rx =*/A1, /*tx =*/A0);
  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
  }
  Serial.println(F("DFPlayer Mini online."));

  String response = getRequest("https://jsonplaceholder.typicode.com/todos/1");
  JSONVar json = JSON.parse(response);

  if (JSON.typeof(json) == "undefined") {
    Serial.println("Could not parse input.");
    return;
  }

  Serial.println(getProperty(json, "id"));
  Serial.println(getProperty(json, "title"));
}

// Continuously updates the LED and alarm based on active settings
void loop() {
  // updateLED();
  // loopAlarm();
  // delay(1);
}

// Update the LED based on active settings. When alarm is active, flash LED. When inactive, allow manual control via website
void updateLED() {
  // Active alarm mode
  if (alarmMode == "active") {
    analogWrite(13, redBaseBrightness * (float(brightness) / 255) * brightnessFactor);
    analogWrite(12, greenBaseBrightness * (float(brightness) / 255) * brightnessFactor);
    analogWrite(27, blueBaseBrightness * (float(brightness) / 255) * brightnessFactor);
    brightness = brightness + fadeAmount;

    if (brightness < 0) {
      brightness = 0;
      fadeAmount = -fadeAmount;
    }

    if (brightness > 255) {
      brightness = 255;
      fadeAmount = -fadeAmount;
    }
  // Inactive alarm mode
  } else {
    if (ledState == "off") {
      analogWrite(13, 0);
      analogWrite(12, 0);
      analogWrite(27, 0);
    } else {
      analogWrite(13, redBaseBrightness * brightnessFactor);
      analogWrite(12, greenBaseBrightness * brightnessFactor);
      analogWrite(27, blueBaseBrightness * brightnessFactor);
    }
  }
}

// Loop the alarm continuously if the alarm is active
void loopAlarm() {
  if (alarmMode == "active") {
    if (millis() - timer >= TIMER_INTERVAL) {
      myDFPlayer.play(1);
      timer = millis();
    }
  }
}

String getRequest(String serverName) {
  HTTPClient http;
  String response = "{}";

  http.begin(serverName.c_str());
  int responseCode = http.GET();

  if (responseCode > 0) {
    response = http.getString();
  } else {
    http.end();
    return String(responseCode);
  }

  http.end();
  return response;
}

String getProperty(JSONVar json, String key) {
  JSONVar keys = json.keys();
  for (int i = 0; i < keys.length(); i++) {
     if key.equalsIgnoreCase(keys[i]) {
       return json[keys[i]];
     }
   }
   Serial.println(keys[0]);
}
