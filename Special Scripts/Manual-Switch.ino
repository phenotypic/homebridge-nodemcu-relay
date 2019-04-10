#include <ESP8266WiFi.h>

// GitHub Page = https://github.com/Tommrodrigues/homebridge-nodemcu-relay

// Script Type = Relay Switch with manual override

// D4 = LOW activation
// D7 = HIGH activation

/////////////////// CHANGE THESE VALUES //////////////////////
const char* ssid = "SSID"; //Name of your network
const char* password = "PASSWORD"; //Password for your network
//////////////////////////////////////////////////////////////

const int manualSensor = 16; //Declaers "manualSensor" being pin 16 (D0)
const int highPin = 13; //Declares "highPin" being pin 13 (D7) on NodeMCU
const int lowPin = 2; //Declaers "lowPin" being pin 2 (D4) on NodeMCU
const int redPin = 16; //Declaers "redPin" being pin 16 (Red LED)

WiFiServer server(80);
int oldVal = digitalRead(manualSensor);
int currentVal = digitalRead(manualSensor);

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(manualSensor, INPUT);
  digitalWrite(manualSensor, HIGH);
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

}

void loop() {

  int state = LOW;

  state = digitalRead(highPin);
  currentVal = digitalRead(manualSensor);

  if (currentVal != oldVal) {
    if (state == HIGH) {
    digitalWrite(highPin, LOW);
    digitalWrite(lowPin, HIGH);
    oldVal = digitalRead(manualSensor);
    } else {
    digitalWrite(highPin, HIGH);
    digitalWrite(lowPin, LOW);
    oldVal = digitalRead(manualSensor);
    }
  }

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

  if (request.indexOf("/SWITCH=ON") != -1)  {
    digitalWrite(highPin, HIGH);
    digitalWrite(lowPin, LOW);
  }
  if (request.indexOf("/SWITCH=OFF") != -1)  {
    digitalWrite(highPin, LOW);
    digitalWrite(lowPin, HIGH);
  }

  state = digitalRead(highPin);
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");

  if(state == HIGH) {
    client.print("1");
  } else {
    client.print("0");
  }

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

oldVal = digitalRead(manualSensor);
}
