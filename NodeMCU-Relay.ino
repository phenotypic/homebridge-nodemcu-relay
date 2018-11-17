#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

// GitHub Page = https://github.com/Tommrodrigues/homebridge-nodemcu-relay

// Script Type = Relay Momentary, Switch, Modulation

// D4 = LOW activation
// D7 = HIGH activation

/////////////////// CHANGE THESE VALUES //////////////////////
// Required:
const char* ssid = "SSID"; //Name of your network
const char* password = "PASSWORD"; //Password for your network
// For Modulation:
const uint32_t ON_TIME  = 5000; //Amount of time for relay to be on for MODULATION (ms)
const uint32_t OFF_TIME = 10000; //Amount of time for relay to be off for MODULATION (ms)
// For Momentary:
const int delayTimeOn = 1000; //Delay time for the on state for MOMENTARY (ms)
const int delayTimeOff = 1000; //Delay time for the off state for MOMENTARY (ms)
//////////////////////////////////////////////////////////////

int value = LOW;

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

  // Start the mDNS responder for nodemcu.local
  if (!MDNS.begin("nodemcu")) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");

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
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Match the request

    if (request.indexOf("/MODULATION=ON") != -1)  {
       ignoreMe = false;
       start_blinking();
       value = HIGH;
    }
    if (request.indexOf("/MODULATION=OFF") != -1)  {
       ignoreMe = false;
       stop_blinking();
       value = LOW;
    }
    if (request.indexOf("/MOMENTARY=ON") != -1)  {
      stop_blinking();
      digitalWrite(lowPin, LOW);
      digitalWrite(highPin, HIGH);
      delay(delayTimeOn);
      digitalWrite(highPin, LOW);
      digitalWrite(lowPin, HIGH);
      value = HIGH;
      ignoreMe = false;
    }
    if (request.indexOf("/MOMENTARY=OFF") != -1)  {
      stop_blinking();
      digitalWrite(lowPin, LOW);
      digitalWrite(highPin, HIGH);
      delay(delayTimeOff);
      digitalWrite(highPin, LOW);
      digitalWrite(lowPin, HIGH);
      value = LOW;
      ignoreMe = false;
    }
    if (request.indexOf("/SWITCH=ON") != -1)  {
      stop_blinking();
      digitalWrite(highPin, HIGH);
      digitalWrite(lowPin, LOW);
      value = HIGH;
      ignoreMe = true;
    }
    if (request.indexOf("/SWITCH=OFF") != -1)  {
      stop_blinking();
      digitalWrite(highPin, LOW);
      digitalWrite(lowPin, HIGH);
      value = LOW;
      ignoreMe = false;
    }
    if (request.indexOf("/STATE") != -1)  {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      if(value == HIGH) {
        client.print("1");
      } else {
        client.print("0");
      }
      delay(1);
      Serial.println("Client disonnected");
      Serial.println("");
      return;
    }

  // REMOVE EVERYTHING BELOW (UNTIL "END") IF YOU DONT WANT AN ONLINE INTERFACE
  // --------------------------------------------------------------------------
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
  client.println("<a href=\"/MODULATION=ON\"\"><button>Modulation On </button></a>");
  client.println("<a href=\"/MODULATION=OFF\"\"><button>Modulation Off </button></a>");
  
  client.println("<a href=\"/MOMENTARY=ON\"\"><button>Momentary On </button></a>");
  client.println("<a href=\"/MOMENTARY=OFF\"\"><button>Momentary Off </button></a>");
  
  client.println("<a href=\"/SWITCH=ON\"\"><button>Switch On </button></a>");
  client.println("<a href=\"/SWITCH=OFF\"\"><button>Switch Off </button></a><br />");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
  // END
  // --------------------------------------------------------------------------
}
