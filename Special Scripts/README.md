# Special Scripts

As mentioned in the main README, this repository includes a few 'Special Scripts' for more niche use-cases.

## Safe Momentary

`Safe-Momentary.ino` works in much the same way that the `MOMENTARY` function of the main script works. However, the script allows for you to specify a 'code' which will trigger the relay as opposed to simply using `MOMENTARY=<ON/OFF>`. In this sense, `Safe-Momentary.ino` aims to improve on the security of the main script for cases where it may be more of a concern like with garage doors. Note that this 'code' is used for both 'on' and 'off'; despite this, you can still request `/STATE` from the device to recieve either `1` or `0`.

**HOWEVER:** this is, in no way, completely secure and should therefore be treated as such. You must entirely acknowledge the risks associated with using an unprofessionally made script, especially when it is security that you are looking for.
