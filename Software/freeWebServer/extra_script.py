import os
from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()
data_dir = os.path.join(env.subst("$PROJECT_DIR"), "data")

# Ensure that we upload SPIFFS filesystem data
env.Replace(UPLOADCMD="python -m esptool --chip esp32 --port $UPLOAD_PORT --baud $UPLOAD_BAUD write_flash -z 0x110000 $SOURCE")
env.Append(UPLOADELFFLAGS=["--spiffs", data_dir])