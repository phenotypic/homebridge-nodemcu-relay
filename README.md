# homebridge-nodemcu-relay

![GitHub last commit](https://img.shields.io/github/last-commit/Tommrodrigues/homebridge-nodemcu-relay.svg) ![GitHub issues](https://img.shields.io/github/issues/Tommrodrigues/homebridge-nodemcu-relay.svg)

## Description

This script interfaces with a [homebridge](https://github.com/nfarina/homebridge) plugin to expose a relay to Apple's [HomeKit](http://www.apple.com/ios/home/), allowing you to integrate numerous devices into your smart-home.

## Requirements

* NodeMCU

* Relay Module

* Pin header cables

* Micro-USB cable

## How-to

1. First, follow [this](https://gist.github.com/Tommrodrigues/8d9d3b886936ccea9c21f495755640dd) gist which walks you through how to flash a NodeMCU using the Arduino IDE. The `.ino` file reffered to is the `NodeMCU-Relay.ino` file included in this repository

2. Assuming that you already have [homebridge](https://github.com/nfarina/homebridge#installation) set up, the next thing you will have to do is install [homebridge-http](https://github.com/rudders/homebridge-http) using the command:
```
npm install -g homebridge-http
```

3. Finally, update your `config.json` file following the example below, making sure to [adapt it](#available-features) accordingly:

```json
"accessories": [
    {
      "accessory": "Http",
      "name": "Lights",
      "on_url": "relay.local/SWITCH=ON",
      "off_url": "relay.local/SWITCH=OFF",
      "http_method": "GET"
    }
]
```

## Wiring

Depending on which relay module you have, it will either be `HIGH` or `LOW` activation. The `NodeMCU-Relay.ino` script accommodates for both, providing two seperate pins depending on which type of relay you have:

![Diagram](https://image.ibb.co/hEDhFL/Wiring-Relay-Diagram.jpg)


## Available Features

As you can see from the `config.json` example above, the basic format of the HTTP request is the IP address of your NodeMCU followed by the action you wish to execute.

Here is a table which shows you the available relay actions included with the `NodeMCU-Relay.ino` script in this repository which can be included in the `config.json` to control different types of appliances:

| Name | Full URLs | Description | Example Uses |
| --- | --- | --- | --- |
| Switch | `relay.local/SWITCH=ON` `relay.local/SWITCH=OFF` | Will simply turn on/off the relay permanently as per the Home app. | Lights, Faucets, Fans |
| Momentary | `relay.local/MOMENTARY=ON` `relay.local/MOMENTARY=OFF` | Will activate the relay for a brief moment, then deactivate after the amount of time specified in the `NodeMCU-Relay.ino` script. | [Garages, Gates](#alternative-scripts), Buzzers |
| Modulation | `relay.local/MODULATION=ON` `relay.local/MODULATION=OFF` | Will activate then deactivate the relay constantly for the amount of time specified in the `NodeMCU-Relay.ino` script until turned off. | Lights, Sprinkler systems |
| State | `relay.local/STATE` | Will return either `1` (On) or `0` (Off) depending on the relay's current state | N/A |

#### Web Interface

It is also worth noting that the script provides a (very basic) web interface available at the IP address of your NodeMCU. However, it is important to know that using the web interface to control the NodeMCU will not update the state in the Home app unless you utilise the `STATE` feature. However, the interface can be useful if you have people in your home who do not have an Apple device.


## Other features

### Alternative scripts

Whilst it is recommended to simply use the main `NodeMCU-Relay.ino` script due to its versatility, you can find scripts for the individual actions(`MOMENTARY`, `SWITCH` & `MODULATION`) in the _Individual Scripts_ folder.

Furthermore, there are some more specialiased scripts in the _Special Scripts_ folder. There, you can find a script designed specifically for more secure applications like Garage Door openers.

### Using the STATE feature

As mentioned in the [Available Features](#available-features) section, you can request `/STATE` from your NodeMCU to be returned either `1` or `0` depending on the state. This can be useful if you would like to know the state of the relay for smarter integration.
