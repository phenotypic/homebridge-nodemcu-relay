# homebridge-nodemcu-relay
#### A simple yet efficient way to integrate a homebridge-controlled relay into your home.

## What is it?

homebridge-nodemcu-relay allows for the cheap and easy control of a relay via [homebridge](https://github.com/nfarina/homebridge); opening a wide variety of use-cases including Gate/Garage openers, sprinkler control, lights on/off and far more.

## How does it work?

homebridge-nodemcu-relay provides an Arduino script designed to be flashed onto a NodeMCU. After successful flashing, you will be able to quickly and simply integrate it into your pre-existing homebridge setup with the use of an http plugin such as [homebridge-http](https://github.com/rudders/homebridge-http).

## Requirements

* [NodeMCU](https://en.wikipedia.org/wiki/NodeMCU) (Or another [ESP8266](https://en.wikipedia.org/wiki/ESP8266) based board)

* 3v Relay Module

* [Pin Header Cables](https://learn.sparkfun.com/tutorials/connector-basics/pin-header-connectors) (To connect the NodeMCU to the relay module)

* Micro-USB cable (for flashing and power)

* A method for flashing the NodeMCU with Arduino scripts (NodeMCU method described [below](#how-to))

## How-to

The following is the process used to flash a NodeMCU with the Arduino IDE. Steps may vary if you are using a board other than the NodeMCU. For a video of a walkthrough of how to use the Arduino IDE to flash the NodeMCU, click [here](https://www.youtube.com/watch?v=G6CqvhXpBKM&list=LLgF700LhcjqMmwWtcsFs92Q&index=14&t=0s&frags=pl%2Cwn).

1. Download the `NodeMCU-Relay.ino` script from this repository.

2. Download and install the Arduino IDE from [here](https://www.arduino.cc/en/Main/Software).

3. In the Preferences pane, under "Additional Board Manager URLs", enter the following link:
http://arduino.esp8266.com/versions/2.4.2/package_esp8266com_index.json

4. Go to the "Tools" window and select "Board Manager". Search for `ESP8266` and click on install.

5. Ensure to download the USB to UART drivers for your operating system from [this](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers) link in order to be able to flash our NodeMCU.

6. Restart the Arduino IDE.

7. Select NodeMCU from "Tools" then "Boards".

8. Now, copy and paste the contents of the `NodeMCU-Relay.ino` script we downloaded from this repository earlier into the Arduino IDE window.

9. Make sure to change the variables where asked to do so. The variables you can change are surrounded by slashes (`/`) to ensure that they stand out and to help avoid you acciddentaly changing other parts of the script. Below is an example:

```
/////////////////// CHANGE THESE VALUES //////////////////////
// Required:
const char* ssid = "MyWifi"; //Name of your network
const char* password = "examplepassword"; //Password for your network
// For Modulation:
const uint32_t ON_TIME  = 5000; //Amount of time for relay to be on for MODULATION (ms)
const uint32_t OFF_TIME = 10000; //Amount of time for relay to be off for MODULATION (ms)
// For Momentary:
const int delayTimeOn = 1000; //Delay time for the on state for MOMENTARY (ms)
const int delayTimeOff = 1000; //Delay time for the off state for MOMENTARY (ms)
//////////////////////////////////////////////////////////////
```

10. Next, after you have plugged in your NodeMCU using a Micro-USB cable capable of data transfer, click on the "Right Arrow" (➡) icon in the top left of the Arduino IDE to start flashing.

11. Once the flashing has finished, you should see a red LED turn on, on the NodeMCU and turn off shortly after; when the red LED shows, it indicates that the NodeMCU is attempting to connect to the network you specified and the LED will turn off once it has connected successfully.

12. At this point, we have finished configuring the NodeMCU and can focus on setting up homebridge to be able to communicate with the NodeMCU. It is important to note that the NodeMCU should be available as `nodemcu.local` on your network. However, if you want to find its IP address, you can either `ping nodemcu.local` or use the Serial Output feature in the Arduino IDE when the NodeMCU is booting up and plugged into your computer.

13. Assuming that you already have [homebridge](https://github.com/nfarina/homebridge#installation) already set up, the next thing you will have to do is install the http plugin you wish to use. It is recommended that you use [homebridge-http](https://github.com/rudders/homebridge-http) due to its stability and wide usage. To install [homebridge-http](https://github.com/rudders/homebridge-http), use the command:
```
sudo npm install -g homebridge-http
```

14. Below is the example `config.json` file which assumes that you want to use [homebridge-http](https://github.com/rudders/homebridge-http) to communicate with your NodeMCU and that you want to use the switch [feature](#available-features):

```
{
    "bridge": {
        "name": "Homebridge",
        "username": "CC:22:3D:E3:CE:30",
        "port": 51826,
        "pin": "031-45-154"
    },
   

    "accessories": [
        {
          "accessory": "Http",
          "name": "Lights",
          "on_url": "http://nodemcu.local:80/SWITCH=ON",
          "off_url": "http://nodemcu.local:80/SWITCH=OFF",
          "http_method": "GET"
        }
    ],

    "platforms": [
    ]
}
```

15. And that's it! The `config.json` example above will simply turn the relay either on or off depending on what you request due to the fact that `SWITCH` is listed as the action. If you want to do some other sort of action with the relay such as a momentary pulse, read the [Available Features](#available-features) section below.

## Wiring

Depending on which relay module you have, it will either be a `HIGH` activation relay or a `LOW` activation relay. The `NodeMCU-Relay.ino` script accommodates for both, providing two seperate pins depending on which type of relay ou have, follow the correct wiring diagram for you below:

![Diagram](https://image.ibb.co/gSrmRU/Wiring_Relay_Diagram.jpg)


## Available Features

As you can see from the `config.json` example above, the basic format of the HTTP request is `http://nodemcu.local:80/` followed by the action you wish to execute. 

Here is a table which shows you the available relay actions included with the `NodeMCU-Relay.ino` script in this repository which can be included in the `config.json` to control different types of appliances:

| Name | Full URLs | Description | Example Uses |
| --- | --- | --- | --- |
| Switch | `http://nodemcu.local:80/SWITCH=ON` `http://nodemcu.local:80/SWITCH=OFF` | Will simply turn on/off the relay permanently as per the Home app. | Lights, faucets and fans etc. |
| Momentary | `http://nodemcu.local:80/MOMENTARY=ON` `http://nodemcu.local:80/MOMENTARY=OFF` | Will activate the relay for a brief moment then deactivate after the amount of time specified in the `NodeMCU-Relay.ino` script. | Garages, gates and buzzers etc. |
| Modulation | `http://nodemcu.local:80/MODULATION=ON` `http://nodemcu.local:80/MODULATION=OFF` | Will activate then deactivate the relay constantly for the amount of time specified in the `NodeMCU-Relay.ino` script until turned off. | Lights and sprinkler systems etc. |

#### Web Interface

It is also worth noting that the script provides a web interface available at either `nodemcu.local` or the IP address of your NodeMCU. From the (very basic) web interface, you can control any of the features included in the script. However, it is importatnt to know that using the web interface to control the NodeMCU will NOT update the state in the Home app but can be useful if you have people in your home who do not have an Apple device.


## Other features

#### Integrating with a "command" plugin

Whilst the `NodeMCU-Relay.ino` script was designed to be interacted with by an http plugin; it _is_ possible to control the NodeMCU from a "command" type plugin such as [homebridge-garagedoor-command](https://github.com/apexad/homebridge-garagedoor-command) by using:
```
curl http://nodemcu.local:80/ACTION
```
Where `ACTION` is the action you wish to perform (e.g. `SWITCH=ON`).

#### Using a specific action type

Whilst it is highly recommended to simply use the main `NodeMCU-Relay.ino` script due to its versatility due to the fact that it contains all of the action types; if you _do_ wish to only flash the NodeMCU with a specific action (i.e. Switch, Modulation or Momentary), you can find these individual scripts located in the "Other Scripts" folder in this repository.
