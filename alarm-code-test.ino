#include <WiFi.h>
#include <HTTPClient.h>
#include "Arduino.h"
#include <Arduino_JSON.h>
#include "DFRobotDFPlayerMini.h"
#define FPSerial Serial1


  // This is the simple code that the alarm runs on. It initializes an access point at "ESP32-Access-Point" with password "123456789" that you connect to to access its webpage (line 338).
  // There is also commented out code (lines 341 - 350) that can connect to an existing wifi network, so, as long as the client is connected to that network, they can connect to each other that way.
  // When a client connects to the root, the alarm sends back a wbepage to be displayed on the client side (lines 368 - 370). If you want to use a different webpage, comment out that part.
  // You can control the alarm with several endpoints (lines 368 - 513). Here are such endpoints:

    // GET /LED/toggle - toggles the LED on or off when in "inactive" mode
    // GET /LED/white - changes the LED color to white
    // GET /LED/red - changes the LED color to red
    // GET /LED/orange - changes the LED color to orange
    // GET /LED/yellow - changes the LED color to yellow
    // GET /LED/lime - changes the LED color to lime
    // GET /LED/green - changes the LED color to green
    // GET /LED/cyan - changes the LED color to cyan
    // GET /LED/blue - changes the LED color to blue
    // GET /LED/purple - changes the LED color to purple
    // GET /LED/pink - changes the LED color to pink
    // POST /LED/brightness - sets the brightness of the LED to the posted value. Should be between 0.00 and 1.00

    // GET /alarm/activate - plays one iteration of the alarm sound (when the alarm is in "active" mode, it loops the alarm sound infinitely)
    // POST /alarm/volume - sets the volume of the alarm to the posted value. Should be between 0.00 and 1.00

    // GET /mode/toggle - toggles the mode between inactive and active. inactive means the alarm is not going off and you can manually activate either the led or the speaker via
      // the web app. Active means that the alarm is going off.

  // One thing to note about the post requests is that the body should be completely empty except for just the value you want to send. If you want to send a value of 0.5 to
    // GET /LED/brightness, then send the body as 0.5, not value=0.5 or brightness=0.5.

  // Credits to https://randomnerdtutorials.com/esp32-web-server-arduino-ide/, https://randomnerdtutorials.com/esp32-access-point-ap-web-server/,
  // https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/, and ChatGPT.
    


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
  // for (int i = 0; i < keys.length(); i++) {
  //   if key.equalsIgnoreCase(keys[i]) {
  //     return json[keys[i]];
  //   }
  // }
  Serial.println(keys[0]);
}
