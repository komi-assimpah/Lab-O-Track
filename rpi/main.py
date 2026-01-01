import json
import os
from i2c_master import I2CMaster
from arduino_device import ArduinoDevice
from gateway import Gateway
from logger import Logger

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

def load_config(filepath="data/arduinos_config.json"):
    full_path = os.path.join(SCRIPT_DIR, filepath)
    with open(full_path) as f:
        return json.load(f)

def arduino_devices_init(config):
    devices = []
    for d in config["devices"]:
        devices.append(ArduinoDevice(
            id=d["id"],
            name=d["name"],
            address=d["address"],
            timeout_minutes=d["timeout_minutes"],
            toalert_email=config["alerts"]["email"]
        ))
    return devices

def main():
    config = load_config()
    
    i2c = I2CMaster(1)
    logger = Logger()
    devices = arduino_devices_init(config)
    
    gateway = Gateway(i2c, devices, logger)
    
    print(f"Gateway started with {len(devices)} device(s)")
    gateway.run(poll_interval_seconds=5)

if __name__ == "__main__":
    main()