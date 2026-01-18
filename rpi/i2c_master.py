import smbus2

REG_STATUS = 0x00
REG_COMMAND = 0x10

CMD_NOP = 0x00
CMD_STOP_ALARM = 0x01
CMD_CLEAR_TAGS = 0x02
CMD_ADD_TAG = 0x03

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

    def init_allowed_tags(self, address, tags):
        """
        Sends a list of allowed tags to the Arduino.
        """
        try:
            self.send_command(address, CMD_CLEAR_TAGS)
            
            for tag in tags:
                # Limit tag length to 15 chars + null terminator usually handled by C string logic if needed
                # We send raw bytes. Tag should be ascii.
                data = [ord(c) for c in tag[:15]] 
                self.bus.write_i2c_block_data(address, REG_ADD_TAG, data)
                
            print(f"Configured 0x{address:02X} with {len(tags)} tags.")
            return True
        except Exception as e:
            print(f"Error configuring 0x{address:02X}: {e}")
            return False

    def stop_alarm(self, address):
        # TODO: implement a timeout of alarm and send a stop alarm command, and then the email
        return self.send_command(address, CMD_STOP_ALARM)

    def close(self):
        self.bus.close()
