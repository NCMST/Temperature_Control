# Project overview

[Supported boards](https://docs.zephyrproject.org/latest/boards/index.html)

Finde your goard
```bash
west boards | grep esp32
```

## Use the following command to activate the Python virtual environment:

```bash
source ~/zephyrproject/.venv/bin/activate
```

And then use the following command to build the sample hello world application for the ESP32-DevKitC board on app CPU core:

```bash
west build -p always -b esp32_devkitc/esp32/procpu samples/hello_world/
west flash

```

## For WSL 

run the folowing comands on Powershell:
```PowerShell
usbipd list # to identify the device

usbipd bind --busid 1-1 # to bind the device

usbipd attach --wsl --busid 1-1 # to attach the device to WSL

```
on WLS run to check the device:

```bash
ls -la /dev/ttyUSB0
```

## For pweronal project run:

```bash
export ZEPHYR_BASE=~/zephyrproject/zephyr

west build --board esp32_devkitc_wroom/esp32/procpu app -p

west flash --esp-device /dev/ttyUSB0 -- --erase # to erase the flash if use only one core
```