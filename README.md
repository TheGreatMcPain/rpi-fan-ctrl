# A fan controller for the Raspberrypi (Tested on RPI3 B+)

This is a simple fan controller that will turn on a fan at a certain CPU temperature.
Written in C using the pigpio library.

## Building and Installation

Make sure you have pigpio installed on your PI.

```
$ git clone https://gitlab.com/TheGreatMcPain/rpi-fan-ctrl.git
$ cd rpi-fan-ctrl
$ meson build
$ cd build
$ ninja
$ sudo ninja install
```

## Usage

```
# rpi-fan-ctrl <options>
```

| Option | Description |
|:------:| ----------- |
| `-h` | Displays a help message. |
| `-u <temp>` | The CPU temperature threshold where the fan will turn on. (defaults to 50) |
| `-l <temp>` | The CPU temperature threshold where the fan will turn off. (defaults to 40) |
