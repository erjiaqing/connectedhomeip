# Mobile Device Test

## Description

This directory contains the `mobile-device-test.py`, which simulates a mobile
phone app on POSIX platforms using CHIP Device Controller API provided by the
CHIP python package.

This script is desired for testing automatically (for example, the Cirque test
on Github Actions) instead of manaully, so no user input except the command line
arguments is needed.

The script can be used as a start point of writing your own testing scripts.
Thus it provides two help functions (`TestBLEKeyExchange` and
`TestWiFiNetworkCommissioning`) which is not used by the script itself.

## Usage

```
python3 mobile-device-test.py -t <timeout> -a <address>

# Examples
# Do test routine over 192.168.0.2, the test script will return with error after 30 seconds
python3 mobile-device-test.py -t 30 -a 192.168.0.2
```
