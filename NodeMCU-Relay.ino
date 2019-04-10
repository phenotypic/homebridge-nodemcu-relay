#include <ESP8266WiFi.h>

// GitHub Page = https://github.com/Tommrodrigues/homebridge-nodemcu-relay

// Script Type = Relay Momentary, Switch, Modulation

// D4 = LOW activation
// D7 = HIGH activation

/////////////////// CHANGE THESE VALUES //////////////////////
// Required:
const char* ssid = "SSID"; //Name of your network
const char* password = "PASSWORD"; //Password for your network
// For Modulation:
const uint32_t ON_TIME = 5000; //Time (in ms) for relay to be ON when modulating
const uint32_t OFF_TIME = 10000; //Time (in ms) for relay to be OFF when modulating
// For Momentary:
const int delayTimeOn = 1000; //Delay time (in ms) for the ON state for MOMENTARY
const int delayTimeOff = 1000; //Delay time (in ms) for the OFF state for MOMENTARY
//////////////////////////////////////////////////////////////

bool stateBool = false;

bool led_blinking, led_on, ignoreMe;
uint32_t last_toggle;

const int highPin = 13; //Declares "highPin" being pin 13 (D7) on NodeMCU
const int lowPin = 2; //Declaers "lowPin" being pin 2 (D4) on NodeMCU
const int redPin = 16; //Declaers "redPin" being pin 16 (Red LED)
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(lowPin, OUTPUT);
  pinMode(highPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  digitalWrite(highPin, LOW);
  digitalWrite(lowPin, HIGH);
  digitalWrite(redPin, LOW);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);


  WiFi.softAPdisconnect(true);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  digitalWrite(redPin, HIGH);

  ignoreMe = false;
}

//Start of modulation functions
void update_led() {
  uint32_t now = millis();
  if (!led_blinking && !ignoreMe) {
    digitalWrite(highPin, LOW);
    digitalWrite(lowPin, HIGH);
    led_on = false;
    last_toggle = now - OFF_TIME;
    return;
  }
  if (led_on && now - last_toggle >= ON_TIME && !ignoreMe) {
    digitalWrite(highPin, LOW);
    digitalWrite(lowPin, HIGH);
    led_on = false;
    last_toggle = now;
  }
  if (!led_on && now - last_toggle >= OFF_TIME && !ignoreMe) {
    digitalWrite(highPin, HIGH);
    digitalWrite(lowPin, LOW);
    led_on = true;
    last_toggle = now;
  }
}

void start_blinking() {
  digitalWrite(highPin, HIGH);
  digitalWrite(lowPin, LOW);
  led_blinking = true;
  led_on = true;
  last_toggle = millis();
}

void stop_blinking() {
  digitalWrite(highPin, LOW);
  digitalWrite(lowPin, HIGH);
  led_blinking = false;
  led_on = false;
}
//End of modulation functions

//Main loop
void loop() {

  update_led();

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
    stateBool = true;
  }
  if (request.indexOf("/MODULATION=OFF") != -1) {
    ignoreMe = false;
    stop_blinking();
    stateBool = false;
  }
  if (request.indexOf("/MOMENTARY=ON") != -1) {
    stop_blinking();
    digitalWrite(lowPin, LOW);
    digitalWrite(highPin, HIGH);
    delay(delayTimeOn);
    digitalWrite(highPin, LOW);
    digitalWrite(lowPin, HIGH);
    stateBool = true;
    ignoreMe = false;
  }
  if (request.indexOf("/MOMENTARY=OFF") != -1) {
    stop_blinking();
    digitalWrite(lowPin, LOW);
    digitalWrite(highPin, HIGH);
    delay(delayTimeOff);
    digitalWrite(highPin, LOW);
    digitalWrite(lowPin, HIGH);
    stateBool = false;
    ignoreMe = false;
  }
  if (request.indexOf("/SWITCH=ON") != -1) {
    stop_blinking();
    digitalWrite(highPin, HIGH);
    digitalWrite(lowPin, LOW);
    stateBool = true;
    ignoreMe = true;
  }
  if (request.indexOf("/SWITCH=OFF") != -1) {
    stop_blinking();
    digitalWrite(highPin, LOW);
    digitalWrite(lowPin, HIGH);
    stateBool = false;
    ignoreMe = false;
  }
  if (request.indexOf("/STATE") != -1) {
    client.println(String(stateBool));
    delay(1);
    Serial.println("Client disonnected");
    Serial.println("");
    return;
  }

  // REMOVE EVERYTHING BELOW (UNTIL "END") IF YOU DONT WANT AN ONLINE INTERFACE
  // --------------------------------------------------------------------------
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<h1>NodeMCU Relay</h1>");
  client.println("<h3><u>Current state: " + String(stateBool) + "</u></h3>");

  client.println("<b>Modulation</b>");
  client.println("<a href=\"/MODULATION=ON\"><button>On</button></a>");
  client.println("<a href=\"/MODULATION=OFF\"><button>Off</button></a>");
  client.println("<br><br>");

  client.println("<b>Momentary</b>");
  client.println("<a href=\"/MOMENTARY=ON\"><button>On</button></a>");
  client.println("<a href=\"/MOMENTARY=OFF\"><button>Off</button></a>");
  client.println("<br><br>");

  client.println("<b>Switch</b>");
  client.println("<a href=\"/SWITCH=ON\"><button>On</button></a>");
  client.println("<a href=\"/SWITCH=OFF\"><button>Off</button></a>");
  client.println("</html>");
  // --------------------------------------------------------------------------
  // END

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
