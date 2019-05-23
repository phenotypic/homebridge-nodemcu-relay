# Special Scripts

As mentioned in the main README, this repository includes a few 'Special Scripts' for more niche use-cases.

## Safe Momentary

`Safe-Momentary.ino` works in much the same way that the `MOMENTARY` function of the main script works. However, the script allows for you to specify a `key` which will trigger the relay as opposed to simply using `MOMENTARY=<ON/OFF>`. In this sense, `Safe-Momentary.ino` aims to improve on the security of the main script for cases where it may be more of a concern like with garage doors. Note that this 'code' is used for both 'on' and 'off' but you can still request `/STATE` from the device to recieve either `1` or `0`.

**NOTE:** This script is only a **mild improvement** over the standard script and will open any time your `key` is requested. Therefore, anyone on your network who knows the key can activate the script. Acquiring the key is trivial once inside your network by running a simple Wireshark scan. As such, the 'security' of this script **relies entirely on your network security** (e.g. the strength of your Wi-Fi password).
