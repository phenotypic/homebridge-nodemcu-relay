# homebridge-nodemcu-relay

This script interfaces with [homebridge](https://github.com/homebridge/homebridge) to expose a relay to Apple's [HomeKit](http://www.apple.com/ios/home/), allowing you to integrate numerous devices into your smart home.

## Requirements

* NodeMCU

* Relay Module

* Pin header cables

* Micro-USB cable

## How-to

1. First, install the `ArduinoJson` library from the _Library manager_ in the Arduino IDE, then follow [this](https://gist.github.com/phenotypic/8d9d3b886936ccea9c21f495755640dd) gist which walks you through how to flash a NodeMCU.

2. Assuming that you already have [homebridge](https://github.com/homebridge/homebridge#installation) set up, the next thing you will have to do is install [homebridge-http](https://github.com/rudders/homebridge-http) using the command:
```
npm install -g homebridge-http
```

3. Finally, update your `config.json` file following the example below, making sure to [adapt it](#available-features) accordingly:

```json
"accessories": [
    {
      "accessory": "Http",
      "name": "Lights",
      "on_url": "http://relay.local/setState?type=switch&value=1",
      "off_url": "http://relay.local/setState?type=switch&value=0",
      "http_method": "GET"
    }
]
```

## Wiring

| NodeMCU | Relay Module |
| --- | --- |
| `3V3` | `VCC` |
| `GND` | `GND` |
| `D7` | `IN1` |

## Available Features

As you can see from the `config.json` example above, the basic format of the HTTP request is the declaration of the mode type followed by the required state:
```
http://relay.local/setState?type=STRING_VALUE&value=INT_VALUE
```

Here is a table which shows you the available queries:

| Name | Query | Description | Example Uses |
| --- | --- | --- | --- |
| Switch | `/setState?type=switch` | Will simply turn on/off the relay permanently as per the Home app. | Lights, Faucets, Fans |
| Momentary | `/setState?type=momentary` | Will activate the relay for a brief moment, then deactivate after the amount of time specified in the script. | Garages, Gates, Buzzers |
| Modulation | `/setState?type=modulation` | Will activate then deactivate the relay constantly for the amount of time specified in the script until turned off. | Lights, Misting systems |
| State | `/status` | Will return the `currentState` in JSON form as `0` or `1` | N/A |
