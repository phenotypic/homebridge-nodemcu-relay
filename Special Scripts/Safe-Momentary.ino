#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

// GitHub Page = https://github.com/Tommrodrigues/homebridge-nodemcu-relay

// Script Type = Relay Momentary (For sensitive devices e.g. Garage Doors)

// D4 = LOW activation
// D7 = HIGH activation

/////////////////// CHANGE THESE VALUES //////////////////////
const char* ssid = "SSID"; //Name of your network
const char* password = "PASSWORD"; //Password for your network
const char* mdns = "relay"; //mDNS name
const char* key = "/SECRETKEY"; //e.g. "/2148320" or "/IOHEWohVuw"
const int delayTimeOn = 1000; //Delay time for the on state (ms)
//////////////////////////////////////////////////////////////

bool value = false;

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
  Serial.println("Connecting to \"" + String(ssid) + "\"");

  WiFi.softAPdisconnect(true);
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

  digitalWrite(redPin, HIGH);

}

void loop() {

  MDNS.update();

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("New client");
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Match the request

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");

  if (request.indexOf(key) != -1)  {
    digitalWrite(lowPin, LOW);
    digitalWrite(highPin, HIGH);
    delay(delayTimeOn);
    digitalWrite(highPin, LOW);
    digitalWrite(lowPin, HIGH);
    value = !value;
    client.print("Done");
  }


  if (request.indexOf("/STATE") != -1)  {
    if(value) {
      client.print("1");
    } else {
      client.print("0");
    }
  }

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

}
