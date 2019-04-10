#include <ESP8266WiFi.h>

// GitHub Page = https://github.com/Tommrodrigues/homebridge-nodemcu-relay

// Script Type = Relay Modulation

// D4 = LOW activation
// D7 = HIGH activation

/////////////////// CHANGE THESE VALUES //////////////////////
const char* ssid = "SSID"; //Name of your network
const char* password = "PASSWORD"; //Password for your network
const uint32_t ON_TIME  = 5000; //Amount of time for relay to be on (ms)
const uint32_t OFF_TIME = 10000; //Amount of time for relay to be off (ms)
//////////////////////////////////////////////////////////////

bool led_blinking;
bool led_on;
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

}

//START OF MODULATION FUNCTIONS
void update_led() {
  uint32_t now = millis();
    if (!led_blinking) {
        digitalWrite(highPin, LOW);
        digitalWrite(lowPin, HIGH);
        led_on = false;
        last_toggle = now - OFF_TIME;
        return;
    }
    if (led_on && now - last_toggle >= ON_TIME) {
        digitalWrite(highPin, LOW);
        digitalWrite(lowPin, HIGH);
        led_on = false;
        last_toggle = now;
    }
    if (!led_on && now - last_toggle >= OFF_TIME) {
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
//END OF MODULATION FUNCTIONS

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
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Match the request
int value = LOW;
    if (request.indexOf("/MODULATION=ON") != -1)  {
       start_blinking();
       value = HIGH;
    }
    if (request.indexOf("/MODULATION=OFF") != -1)  {
       stop_blinking();
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
  client.println("<a href=\"/MODULATION=ON\"\"><button>Turn On </button></a>");
  client.println("<a href=\"/MODULATION=OFF\"\"><button>Turn Off </button></a><br />");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

}
