#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

// GitHub Page = https://github.com/Tommrodrigues/homebridge-nodemcu-relay

// Script Type = Relay Momentary, Switch, Modulation

// D7 = Relay

/////////////////// CHANGE THESE VALUES //////////////////////
// Required:
const char* ssid = "SSID"; // Name of your network
const char* password = "PASSWORD"; // Password for your network
const String relay = "HIGH"; // Relay type (`HIGH` or `LOW`)
const char* mdns = "relay"; // mDNS name
// For Modulation:
const uint32_t modulationOn = 5000; // Time (in ms) for relay to be ON when modulating
const uint32_t modulationOff = 20000; // Time (in ms) for relay to be OFF when modulating
// For Momentary:
const int momentaryOn = 1000; // Delay time (in ms) for the ON state for MOMENTARY
const int momentaryOff = 1000; // Delay time (in ms) for the OFF state for MOMENTARY
//////////////////////////////////////////////////////////////

const int relayPin = 13;
int state = 0;
bool ignoreMe = false;

int relayOn, relayOff;
bool led_blinking, led_on;
uint32_t last_toggle;

ESP8266WebServer server(80);

void setup() {
  if (relay.equals("LOW")) {
    relayOn = 0;
    relayOff = 1;
  } else {
    relayOn = 1;
    relayOff = 0;
  }

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, relayOff);

  Serial.begin(115200);
  delay(10);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.println("Connecting to \"" + String(ssid) + "\"");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(String(++i) + " ");
  }
  Serial.println();
  Serial.println("Connected successfully");

  // Print the IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin(mdns)) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS address: " + String(mdns) + ".local");

  server.on("/setState", []() {
    String type = server.arg("type");
    state = server.arg("value").toInt();
    if (relay.equals("modulation")) {
      if (state) {
        ignoreMe = false;
        start_blinking();
      } else {
        ignoreMe = false;
        stop_blinking();
      }
    }

    if (relay.equals("momentary")) {
      if (state) {
        stop_blinking();
        digitalWrite(relayPin, relayOn);
        delay(momentaryOn);
        digitalWrite(relayPin, relayOff);
        ignoreMe = false;
      } else {
        stop_blinking();
        digitalWrite(relayPin, relayOn);
        delay(momentaryOff);
        digitalWrite(relayPin, relayOff);
        ignoreMe = false;
      }
    }

    if (relay.equals("switch")) {
      if (state) {
        stop_blinking();
        digitalWrite(relayPin, relayOn);
        ignoreMe = true;
      } else {
        stop_blinking();
        digitalWrite(relayPin, relayOff);
        ignoreMe = false;
      }
    }
    server.send(200);
  });

  server.on("/status", []() {
    size_t capacity = JSON_OBJECT_SIZE(1);
    DynamicJsonDocument doc(capacity);

    doc["currentState"] = state;

    String json;
    serializeJson(doc, json);
    server.send(200, "application/json", json);
  });

  // Start the server
  server.begin();
}

//Start of modulation functions
void update_led() {
  uint32_t now = millis();
  if (!led_blinking && !ignoreMe) {
    digitalWrite(relayPin, relayOff);
    led_on = false;
    last_toggle = now - modulationOff;
    return;
  }
  if (led_on && now - last_toggle >= modulationOn && !ignoreMe) {
    digitalWrite(relayPin, relayOff);
    led_on = false;
    last_toggle = now;
  }
  if (!led_on && now - last_toggle >= modulationOff && !ignoreMe) {
    digitalWrite(relayPin, relayOn);
    led_on = true;
    last_toggle = now;
  }
}

void start_blinking() {
  digitalWrite(relayPin, relayOn);
  led_blinking = true;
  led_on = true;
  last_toggle = millis();
}

void stop_blinking() {
  digitalWrite(relayPin, relayOff);
  led_blinking = false;
  led_on = false;
}
//End of modulation functions

//Main loop
void loop() {
  update_led();
  server.handleClient();
  MDNS.update();
}
