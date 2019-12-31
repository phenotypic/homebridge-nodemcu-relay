#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

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

WiFiServer server(80);

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

  // Start the server
  server.begin();

  // Print the IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin(mdns)) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS address: " + String(mdns) + ".local");
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

  MDNS.update();

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("New client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");

  // Match the request
  if (request.indexOf("/MODULATION=ON") != -1) {
    ignoreMe = false;
    start_blinking();
    state = 1;
  }

  if (request.indexOf("/MODULATION=OFF") != -1) {
    ignoreMe = false;
    stop_blinking();
    state = 0;
  }

  if (request.indexOf("/MOMENTARY=ON") != -1) {
    stop_blinking();
    digitalWrite(relayPin, relayOn);
    delay(momentaryOn);
    digitalWrite(relayPin, relayOff);
    state = 1;
    ignoreMe = false;
  }

  if (request.indexOf("/MOMENTARY=OFF") != -1) {
    stop_blinking();
    digitalWrite(relayPin, relayOn);
    delay(momentaryOff);
    digitalWrite(relayPin, relayOff);
    state = 0;
    ignoreMe = false;
  }

  if (request.indexOf("/SWITCH=ON") != -1) {
    stop_blinking();
    digitalWrite(relayPin, relayOn);
    state = 1;
    ignoreMe = true;
  }

  if (request.indexOf("/SWITCH=OFF") != -1) {
    stop_blinking();
    digitalWrite(relayPin, relayOff);
    state = 0;
    ignoreMe = false;
  }

  if (request.indexOf("/status") != -1) {
    client.print(String(state));
  }

  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}
