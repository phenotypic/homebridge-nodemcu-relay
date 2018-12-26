# Special Scripts

As mentioned in the main README, this repository includes a few 'Special Scripts' for more niche use-cases.

## Safe Momentary

`Safe-Momentary.ino` works in much the same way that the `MOMENTARY` function of the main script works. However, the script allows for you to specify a 'code' which will trigger the relay as opposed to simply using `MOMENTARY=<ON/OFF>`. In this sense, `Safe-Momentary.ino` aims to improve on the security of the main script for cases where it may be more of a concern like with garage doors. Note that this 'code' is used for both 'on' and 'off'; despite this, you can still request `/STATE` from the device to recieve either `1` or `0`.

**HOWEVER:** this is, in no way, completely secure and should therefore be treated as such. You must entirely acknowledge the risks associated with using an unprofessionally made script, especially when it is security that you are looking for.

## Manual Switch (Untested)

This script aims to enable you to automate pre-existing light switches in your home. Here is a wiring diagram:

![Diagram](https://i.ibb.co/f4317Ht/Manual-Switch-Wiring-Diagram.jpg)

The idea is that you place the above circuit behind a light switch, connect the old switch to `D0` and `GND` and wire the lights through the relay. However, this may be difficult depending on how your lighting is wired; be sure you know what you're doing.

**NOTE:** Lighting is **MAINS VOLTAGE** and can be **HARMFUL**; therefore, you should not intereact with it unless you genuinely know what you're doing! Furthermore, depending on where you live, this could be considered illegal as it may break health and safety laws.

Here is an example `config.json` file which makes use of [homebridge-http-switch](https://github.com/Supereg/homebridge-http-switch):

```json
{
    "bridge": {
        "name": "Homebridge",
        "username": "CC:22:3D:E3:CE:30",
        "port": 51826,
        "pin": "031-45-154"
    },

    "accessories": [
        {
          "accessory": "HTTP-SWITCH",
          "name": "Switch",
	  "switchType": "stateful",
          "onUrl": "http://nodemcu.local/SWITCH=ON",
          "offUrl": "http://nodemcu.local/SWITCH=OFF",
          "statusUrl": "http://nodemcu.local",
          "pullInterval": 5000
        }
    ],

    "platforms": [
    ]
}
```
