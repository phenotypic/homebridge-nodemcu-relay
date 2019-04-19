#include <ESP8266WiFi.h>

// GitHub Page = https://github.com/Tommrodrigues/homebridge-nodemcu-relay

// Script Type = Relay Momentary

// D4 = LOW activation
// D7 = HIGH activation

/////////////////// CHANGE THESE VALUES //////////////////////
const char* ssid = "SSID"; //Name of your network
const char* password = "PASSWORD"; //Password for your network
const int delayTimeOn = 1000; //Delay time for the on state (ms)
const int delayTimeOff = 1000; //Delay time for the off state (ms)
//////////////////////////////////////////////////////////////

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

  digitalWrite(redPin, HIGH);

}

void loop() {
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

  int value = LOW;
  if (request.indexOf("/MOMENTARY=ON") != -1)  {
    digitalWrite(lowPin, LOW);
    digitalWrite(highPin, HIGH);
    delay(delayTimeOn);
    digitalWrite(highPin, LOW);
    digitalWrite(lowPin, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/MOMENTARY=OFF") != -1)  {
    digitalWrite(lowPin, LOW);
    digitalWrite(highPin, HIGH);
    delay(delayTimeOff);
    digitalWrite(highPin, LOW);
    digitalWrite(lowPin, HIGH);
    value = LOW;
  }

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  client.print("Current state: ");

  if(value == HIGH) {
    client.print("On");
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("<a href=\"/MOMENTARY=ON\"\"><button>Turn On </button></a>");
  client.println("<a href=\"/MOMENTARY=OFF\"\"><button>Turn Off </button></a><br />");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

}
