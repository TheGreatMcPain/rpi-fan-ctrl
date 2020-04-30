# A fan controller for the Raspberrypi (Tested on RPI3 B+)

This is a simple fan controller that will turn on a fan at a certain CPU temperature.
Written in C using the pigpio library.

## Building and Installation

Make sure you have pigpio installed on your PI.

`$ git clone https://gitlab.com/TheGreatMcPain/rpi-fan-ctrl.git`\
`$ cd rpi-fan-ctrl`\
`$ make`\
`$ sudo make install`

It should install itself to `/usr/local/sbin`, but you can change the default `PREFIX`,
which is `/usr/local` like this.: `PREFIX=/path/to/prefix make install`

## Usage

`# rpi-fan-ctrl <options>`

| Option | Description |
|:------:| ----------- |
| `-h` | Displays a help message. |
| `-t <temp>` | The CPU temperature threshold where the fan will turn on, or off. (defaults to 50) |
