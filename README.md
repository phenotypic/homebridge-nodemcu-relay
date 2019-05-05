# homebridge-nodemcu-relay

![GitHub last commit](https://img.shields.io/github/last-commit/Tommrodrigues/homebridge-nodemcu-relay.svg) ![GitHub issues](https://img.shields.io/github/issues/Tommrodrigues/homebridge-nodemcu-relay.svg)

## Description

This script interfaces with a [homebridge](https://github.com/nfarina/homebridge) plugin to expose a relay to Apple's [HomeKit](http://www.apple.com/ios/home/), allowing you to integrate numerous devices into your smart-home.

## Requirements

* NodeMCU

* 3V Relay Module

* Pin header cables

* Micro-USB cable

* A method for flashing the NodeMCU with Arduino scripts (NodeMCU method described [below](#how-to))

## How-to

1. First, follow [this](https://gist.github.com/Tommrodrigues/8d9d3b886936ccea9c21f495755640dd) gist which walks you through how to flash a NodeMCU using the Arduino IDE. The `.ino` file reffered to is the `NodeMCU-Relay.ino` file included in this repository

2. Assuming that you already have [homebridge](https://github.com/nfarina/homebridge#installation) set up, the next thing you will have to do is install the http plugin you want to use. It is recommended that you use [homebridge-http](https://github.com/rudders/homebridge-http) due to its stability and widespread usage. To install [homebridge-http](https://github.com/rudders/homebridge-http), use the command:
```
sudo npm install -g homebridge-http
```

3. Below is the example `config.json` file which assumes that you want to use [homebridge-http](https://github.com/rudders/homebridge-http) to communicate with your NodeMCU, and that you want to use the `SWITCH` [feature](#available-features):

```json
"accessories": [
    {
      "accessory": "Http",
      "name": "Lights",
      "on_url": "http://IP_ADDRESS/SWITCH=ON",
      "off_url": "http://IP_ADDRESS/SWITCH=OFF",
      "http_method": "GET"
    }
]
```

4. Simply change `IP_ADDRESS` to the IP of your NodeMCU and that's it! The `config.json` example above will simply turn the relay either on or off depending on what you request due to the fact that `SWITCH` is listed as the action. If you want to do another action with the relay like a momentary pulse, read the [Available Features](#available-features) section below.

## Wiring

Depending on which relay module you have, it will either be `HIGH` or `LOW` activation. The `NodeMCU-Relay.ino` script accommodates for both, providing two seperate pins depending on which type of relay you have:

![Diagram](https://image.ibb.co/hEDhFL/Wiring-Relay-Diagram.jpg)


## Available Features

As you can see from the `config.json` example above, the basic format of the HTTP request is the IP address of your NodeMCU (`IP_ADDRESS`), followed by the action you wish to execute. 

Here is a table which shows you the available relay actions included with the `NodeMCU-Relay.ino` script in this repository which can be included in the `config.json` to control different types of appliances:

| Name | Full URLs | Description | Example Uses |
| --- | --- | --- | --- |
| Switch | `http://IP_ADDRESS/SWITCH=ON` `http://IP_ADDRESS/SWITCH=OFF` | Will simply turn on/off the relay permanently as per the Home app. | Lights, Faucets, Fans |
| Momentary | `http://IP_ADDRESS/MOMENTARY=ON` `http://IP_ADDRESS/MOMENTARY=OFF` | Will activate the relay for a brief moment, then deactivate after the amount of time specified in the `NodeMCU-Relay.ino` script. | [Garages, Gates](#alternative-scripts), Buzzers |
| Modulation | `http://IP_ADDRESS/MODULATION=ON` `http://IP_ADDRESS/MODULATION=OFF` | Will activate then deactivate the relay constantly for the amount of time specified in the `NodeMCU-Relay.ino` script until turned off. | Lights, Sprinkler systems |
| State | `http://IP_ADDRESS/STATE` | Will return either `1` (On) or `0` (Off) depending on the relay's current state | N/A |

#### Web Interface

It is also worth noting that the script provides a web interface available at the IP address of your NodeMCU. From the (very basic) web interface, you can control any of the features included in the script. However, it is importatnt to know that using the web interface to control the NodeMCU may not update the state in the Home app depending on the plugin being used. However, the interface can be useful if you have people in your home who do not have an Apple device.


## Other features

### Alternative scripts

Whilst it is **highly** recommended to simply use the main `NodeMCU-Relay.ino` script due to its versatility, you can also find some more specialiased scripts in the `Special Scripts` folder. There, you can find a script designed specifically for more secure applications like Garage Door openers. There is also a script which could be used to automate pre-existing light switches in your home.

Furthermore, if you _do_ wish to only flash the NodeMCU with a specific action (i.e. Switch, Modulation or Momentary), you can find these individual scripts located in the `Other Scripts` folder in this repository. 

### Integrating with a 'command' plugin

Whilst the `NodeMCU-Relay.ino` script was designed to be interacted with by an http plugin, it is possible to control the NodeMCU from a 'command' type plugin such as [homebridge-garagedoor-command](https://github.com/apexad/homebridge-garagedoor-command) by using:
```
curl http://IP_ADDRESS/ACTION
```
Where `ACTION` is the action you wish to perform (e.g. `MOMENTARY=ON`).

### Using the STATE feature

As mentioned in the [Available Features](#available-features) section, you can request `/STATE` from your NodeMCU to be returned either `1` or `0` depending on the state. This can be useful if you would like to know the state of the relay for smart integration.
