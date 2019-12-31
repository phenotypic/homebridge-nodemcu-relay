# homebridge-nodemcu-relay

![GitHub last commit](https://img.shields.io/github/last-commit/Tommrodrigues/homebridge-nodemcu-relay.svg) ![GitHub issues](https://img.shields.io/github/issues/Tommrodrigues/homebridge-nodemcu-relay.svg)

## Description

This script interfaces with [homebridge](https://github.com/nfarina/homebridge) to expose a relay to Apple's [HomeKit](http://www.apple.com/ios/home/), allowing you to integrate numerous devices into your smart home.

## Requirements

* NodeMCU

* Relay Module

* Pin header cables

* Micro-USB cable

## How-to

1. First, follow [this](https://gist.github.com/Tommrodrigues/8d9d3b886936ccea9c21f495755640dd) gist which walks you through how to flash a NodeMCU using the Arduino IDE. The `.ino` file referred to is the `NodeMCU-Relay.ino` file included in this repository

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
      "on_url": "http://relay.local/SWITCH=ON",
      "off_url": "http://relay.local/SWITCH=OFF",
      "http_method": "GET"
    }
]
```

## Wiring

![Diagram](https://i.ibb.co/Jrzr2Hm/68747470733a2f2f696d6167652e6962622e636f2f68454468464c2f576972696e672d52656c61792d4469616772616d2e6a7067.jpg)


## Available Features

As you can see from the `config.json` example above, the basic format of the HTTP request is the IP address of your NodeMCU followed by the action you wish to execute.

Here is a table which shows you the available relay actions included with the `NodeMCU-Relay.ino` script in this repository which can be included in the `config.json` to control different types of appliances:

| Name | Full URLs | Description | Example Uses |
| --- | --- | --- | --- |
| Switch | `relay.local/SWITCH=ON` `relay.local/SWITCH=OFF` | Will simply turn on/off the relay permanently as per the Home app. | Lights, Faucets, Fans |
| Momentary | `relay.local/MOMENTARY=ON` `relay.local/MOMENTARY=OFF` | Will activate the relay for a brief moment, then deactivate after the amount of time specified in the `NodeMCU-Relay.ino` script. | Garages, Gates, Buzzers |
| Modulation | `relay.local/MODULATION=ON` `relay.local/MODULATION=OFF` | Will activate then deactivate the relay constantly for the amount of time specified in the `NodeMCU-Relay.ino` script until turned off. | Lights, Sprinkler systems |
| State | `relay.local/status` | Will return current state as `0` or `1` | N/A |
