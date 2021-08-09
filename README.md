# A fan controller for the Raspberrypi (Tested on RPI3 B+)

This is a simple fan controller daemon/client that will turn on a fan at a certain CPU temperature.
Written in C using the pigpio library.

## Building and Installation

Make sure you have pigpio installed on your PI.

```sh
$ git clone https://gitlab.com/TheGreatMcPain/rpi-fan-ctrl.git
$ cd rpi-fan-ctrl
$ meson build
$ cd build
$ ninja
$ sudo ninja install
```

## Usage

The single binary acts as both daemon and client.

To start the daemon use the `-d` option.

Once the daemon is running you can get the status of the daemon like so.

```sh
# rpi-fan-ctrl status
```

The `set-upper <temp>` and `set-lower <temp>` client commands can be used to set the daemon's current tempurature thresholds.

### Parameters and Variables

```sh
# rpi-fan-ctrl <options>
```

| Option | Description |
|:------:| ----------- |
| `-h` | Displays a help message. |
| `-d` | Start daemon |

| Daemon Option (requires `-d`) | Description |
|:-----------------------------:| ----------- |
| `-f` | Run daemon in the foreground |
| `-u <temp>` | The CPU temperature threshold where the fan will turn on. (defaults to 50) |
| `-l <temp>` | The CPU temperature threshold where the fan will turn off. (defaults to 40) |

| Client Options | Description |
|:--------------:| ----------- |
| `status` | Get daemon's current status |
| `set-upper <temp>` | Set daemon's upper tempurature threshold |
| `set-lower <temp>` | Set daemon's lower tempurature threshold |

The `RPIFANCTRL_SOCK` environment variable sets the unix socket location (default `/tmp/rpi-fan-ctrl.sock`).
