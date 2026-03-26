# Project overview


Use the following command to activate the Python virtual environment:

```bash
source ~/zephyrproject/.venv/bin/activate
```

And then use the following command to build the sample hello world application for the ESP32-DevKitC board on app CPU core:

```bash
west build -p always -b doit_esp32_devkit_v1/esp32/procpu samples/hello_world/
west flash

```

for pweronal project run:

```bash
export ZEPHYR_BASE=~/zephyrproject/zephyr
west build -p always -b doit_esp32_devkit_v1/esp32/procpu .
west flash
```