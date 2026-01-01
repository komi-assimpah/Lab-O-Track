import smbus2

REG_STATUS = 0x00
REG_COMMAND = 0x10

CMD_NOP = 0x00
CMD_STOP_ALARM = 0x01


class I2CMaster:
    def __init__(self, bus_id=1):
        self.bus = smbus2.SMBus(bus_id)

    def read_status(self, arduino_address):
        try:
            return self.bus.read_byte_data(arduino_address, REG_STATUS)
        except Exception as e:
            print(f"Error while reading I2C 0x{arduino_address}: {e}")
            return None

    def send_command(self, address, command):
        try:
            self.bus.write_byte_data(address, REG_COMMAND, command)
            return True
        except Exception as e:
            print(f"Error while writing I2C 0x{address:02X}: {e}")
            return False

    def stop_alarm(self, address):
        # TODO: implement a timeout of alarm and send a stop alarm command, and then the email
        return self.send_command(address, CMD_STOP_ALARM)

    def close(self):
        self.bus.close()
