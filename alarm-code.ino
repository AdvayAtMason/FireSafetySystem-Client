#include <WiFi.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
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


// Server settings
AsyncWebServer server(80);
String header;

String ledState = "off";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


const String html = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1.0" />
        <style>
            html, body {
                margin: 0;
            }

            #screen {
                background-color: #61636b;
                width: 100vw;
                min-height: 100vh;
                display: flex;
                flex-flow: column nowrap;
                align-items: center;
            }

            #navbar {
                background-color: #dcdde0;
                border-bottom: 2px solid #d2d3d6;
                width: 100vw;
                min-height: clamp(0px, 20vw, 100px);
                display: flex;
                justify-content: space-around;
                position: fixed;
            }

            .nav-button-group {
                min-width: 0px;
                display: flex;
                flex-flow: row nowrap;
                align-items: center;
            }

            .nav-button {
                background-color: white;
                border: 1px solid #555;
                width: clamp(0px, 20vw, 200px);
                height: calc(clamp(0px, 20vw, 200px) * 0.7);
                margin: clamp(0px, 1vw, 15px);
                display: flex;
                justify-content: center;
                align-items: center;
            }

            .nav-button:active {
                background-color: #eee;
            }

            .nav-text {
                font-family: Helvetica;
                font-size: clamp(0px, 4vw, 30px);
                text-align: center;
            }

            #navbar-placeholder {
                min-height: calc(clamp(0px, 20vw, 100px) * 2);
            }

            #content {
                background-color: #c2c2c2;
                border-left: 5px solid #b8b8b8;
                border-right: 5px solid #b8b8b8;
                width: 70vw;
                min-height: 100vh;
                display: flex;
                flex-flow: column nowrap;
                align-items: center;
            }

            button {
                width: clamp(0px, 35vw, 400px);
                height: calc(clamp(0px, 35vw, 400px) * 0.5);
                font-family: Helvetica;
                font-size: clamp(0px, 7vw, 50px);
                color: black;
                margin: clamp(0px, 1vw, 15px);
                background-color: white;
                border-radius: 0;
                border: 1px solid #555;
            }

            button:active {
                background-color: #eee;
            }

            input {
                -webkit-appearance: none;
                width: clamp(0px, 35vw, 400px);
                height: clamp(0px, 2.5vw, 25px);
                background: white;
                outline: none;
                -webkit-transition: .2s;
                transition: opacity .2s;
                border-radius: 100px;
            }

            input::-webkit-slider-thumb {
                -webkit-appearance: none;
                appearance: none;
                width: clamp(0px, 6vw, 40px);
                height: clamp(0px, 6vw, 40px);
                background: #5caee0;
                cursor: pointer;
                border-radius: 100px;
            }

            .slider-container {
                display: flex;
                flex-flow: row nowrap;
                align-items: center;
            }

            .slider-label {
                font-family: Helvetica;
                font-size: clamp(0px, 4vw, 35px);
                margin: clamp(0px, 10px, 35px);
            }

            .divider {
                height: clamp(0px, 20vw, 200px);
                width: 0px;
            }

            #led-settings-screen {
                background-color: #c2c2c2;
                width: 70vw;
                display: flex;
                flex-flow: column nowrap;
                align-items: center;

            }

            #alarm-settings-screen {
                background-color: #c2c2c2;
                width: 70vw;
                display: none;
                flex-flow: column nowrap;
                align-items: center;
            }
        </style>
    </head>
    <body>
        <div id="screen">
            <div id="navbar">
                <div class="nav-button-group">
                    <div class="nav-button" onmouseup="changeScreen('led')">
                        <p class="nav-text">
                            LED Settings
                        </p>
                    </div>
                    <div class="nav-button" onmouseup="changeScreen('alarm')">
                        <p class="nav-text">
                            Alarm Settings
                        </p>
                    </div>
                    <div class="nav-button" id="toggleModeButton" onmouseup="toggleAlarm()">
                        <p class="nav-text" id="toggle-mode-text">
                            Engage Alarm
                        </p>
                    </div>
                </div>
            </div>
            <div id="content">

                <div id="navbar-placeholder"></div>

                <div id="led-settings-screen">
                    <button id="toggleLEDButton" onclick="toggleLED()">OFF</button>

                    <div class="divider"></div>

                    <button onclick="getRequest('/LED/white')">White</button><br>
                    <button onclick="getRequest('/LED/red')">Red</button><br>
                    <button onclick="getRequest('/LED/orange')">Orange</button><br>
                    <button onclick="getRequest('/LED/yellow')">Yellow</button><br>
                    <button onclick="getRequest('/LED/lime')">Lime</button><br>
                    <button onclick="getRequest('/LED/green')">Green</button><br>
                    <button onclick="getRequest('/LED/cyan')">Cyan</button><br>
                    <button onclick="getRequest('/LED/blue')">Blue</button><br>
                    <button onclick="getRequest('/LED/purple')">Purple</button><br>
                    <button onclick="getRequest('/LED/pink')">Pink</button><br>
                    <div class="slider-container">
                        <p class="slider-label">Brightness</p>
                        <p><input type='range' min='0.1' max='1' value='1' step='0.01' oninput="postRequest('/LED/brightness', this.value)"></p>
                    </div>
                </div>
                <div id="alarm-settings-screen">
                    <p><button onclick="getRequest('/alarm/activate')">Play Alarm</button></p>
                    <div class="slider-container">
                        <p class="slider-label">Volume</p>
                        <p><input type="range" min="0.8" max="1" value="1" step="0.01" oninput="postRequest('/alarm/volume', this.value)"></p>
                    </div>
                </div>
            </div>
        </div>
        <script>
            var ledState = 'off';
            var alarmState = "inactive";

            function getRequest(path) {
                const xhr = new XMLHttpRequest();
                xhr.open("GET", path, true);
                xhr.send("\n");
                xhr.send("\n");
            }

            function postRequest(path, value) {
                const xhr = new XMLHttpRequest();
                xhr.open('POST', path, true);
                xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
                xhr.send('data=' + value);
            }

            function toggleLED() {
                if (ledState == 'off') {
                    document.getElementById('toggleLEDButton').innerHTML = 'ON';
                    ledState = 'on';
                } else {
                    document.getElementById('toggleLEDButton').innerHTML = 'OFF';
                    ledState = 'off';
                }
                getRequest("/LED/toggle");
            }

            function toggleAlarm() {
                if (alarmState == "inactive") {
                    alarmState = "active";
                    document.getElementById("toggle-mode-text").innerHTML = "Disengage Alarm";
                } else {
                    alarmState = "inactive";
                    document.getElementById("toggle-mode-text").innerHTML = "Engage Alarm";
                }
                getRequest("/mode/toggle");
            }

            function changeScreen(screen) {
                var ledSettings = document.getElementById("led-settings-screen");
                var alarmSettings = document.getElementById("alarm-settings-screen");
                if (screen == "led") {
                    ledSettings.style.display = "flex";
                    alarmSettings.style.display = "none";
                } else if (screen == "alarm") {
                    ledSettings.style.display = "none";
                    alarmSettings.style.display = "flex";
                }
            }
        </script>
    </body>
</html>
)rawliteral";


void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(27, OUTPUT);

  // Use access point
  Serial.println("Connecting...");
  WiFi.softAP("ESP32-Access-Point", "123456789");
  Serial.println(WiFi.softAPIP());

  // Use WiFi
  // char* ssid = "";
  // char* password = "";
  // Serial.println("Connecting...");
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  // Serial.println(WiFi.localIP());;

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

  // Root, returns the website
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", getHTML());
  });

  // GET /LED/toggle
  server.on("/LED/toggle", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (ledState == "off") {
      Serial.println("LED on");
      ledState = "on";
    } else {
      Serial.println("LED off");
      ledState = "off";
    }
    request->send(200);
  });

  // GET /LED/white
  server.on("/LED/white", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("White");
    redBaseBrightness = 255;
    greenBaseBrightness = 255;
    blueBaseBrightness = 160;
    request->send(200);
  });

  // GET /LED/red
  server.on("/LED/red", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Red");
    redBaseBrightness = 255;
    greenBaseBrightness = 0;
    blueBaseBrightness = 0;
    request->send(200);
  });

  // GET /LED/orange
  server.on("/LED/orange", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Orange");
    redBaseBrightness = 255;
    greenBaseBrightness = 20;
    blueBaseBrightness = 0;
    request->send(200);
  });

  // GET /LED/yellow
  server.on("/LED/yellow", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Yellow");
    redBaseBrightness = 255;
    greenBaseBrightness = 70;
    blueBaseBrightness = 0;
    request->send(200);
  });

  // GET /LED/lime
  server.on("/LED/lime", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Lime");
    redBaseBrightness = 255;
    greenBaseBrightness = 255;
    blueBaseBrightness = 0;
    request->send(200);
  });

  // GET /LED/green
  server.on("/LED/green", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Green");
    redBaseBrightness = 0;
    greenBaseBrightness = 255;
    blueBaseBrightness = 0;
    request->send(200);
  });

  // GET /LED/cyan
  server.on("/LED/cyan", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Cyan");
    redBaseBrightness = 0;
    greenBaseBrightness = 255;
    blueBaseBrightness = 255;
    request->send(200);
  });

  // GET /LED/blue
  server.on("/LED/blue", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Blue");
    redBaseBrightness = 0;
    greenBaseBrightness = 0;
    blueBaseBrightness = 255;
    request->send(200);
  });

  // GET /LED/purple
  server.on("/LED/purple", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Purple");
    redBaseBrightness = 255;
    greenBaseBrightness = 0;
    blueBaseBrightness = 200;
    request->send(200);
  });

  // GET /LED/pink
  server.on("/LED/pink", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Pink");
    redBaseBrightness = 255;
    greenBaseBrightness = 0;
    blueBaseBrightness = 35;
    request->send(200);
  });

  // POST /LED/brightness
  server.on("/LED/brightness", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("Change brightness");
    if (request->hasParam("data", true)) {
      brightnessFactor = request->getParam("data", true)->value().toFloat();
    }
    request->send(200);
  });

  // GET /alarm/activate
  server.on("/alarm/activate", HTTP_GET, [](AsyncWebServerRequest *request) {
    myDFPlayer.play(1);
    request->send(200);
  });

  // POST /alarm/volume
  server.on("/alarm/volume", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("data", true)) {
      alarmVolume = request->getParam("data", true)->value().toFloat();
    }
    myDFPlayer.volume(30 * alarmVolume);
    request->send(200);
  });

  // GET /mode/toggle
  server.on("/mode/toggle", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (alarmMode == "inactive") {
      brightness = 0;
      if (fadeAmount < 0) {
        fadeAmount = fadeAmount * -1;
      }
      timer = millis();
      alarmMode = "active";
      myDFPlayer.play(1);
    } else {
      myDFPlayer.stop();
      alarmMode = "inactive";
    }
    request->send(200);
  });


  // Start server
  server.begin();
}

// Continuously updates the LED and alarm based on active settings
void loop() {
  updateLED();
  loopAlarm();
  delay(1);
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

String getHTML() {
  return html;
}
