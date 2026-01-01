STATUS_TAG_PRESENT = 0x01
STATUS_TIMER_RUNNING = 0x02
STATUS_ALARM_ACTIVE = 0x04


class ArduinoDevice:
    def __init__(self, id, name, address, timeout_minutes, toalert_email):
        self.id = id
        self.name = name
        self.address = address
        self.timeout_minutes = timeout_minutes
        self.toalert_email = toalert_email
        self.last_status = None

    def poll(self, i2c_master):
        self.last_status = i2c_master.read_status(self.address)
        return self.last_status

    def is_tag_present(self):
        return self.last_status is not None and (self.last_status & STATUS_TAG_PRESENT)

    def is_timer_running(self):
        return self.last_status is not None and (self.last_status & STATUS_TIMER_RUNNING)

    def is_alarm_active(self):
        return self.last_status is not None and (self.last_status & STATUS_ALARM_ACTIVE)

    def __tostring__(self):
        return f"ArduinoDevice({self.id}, name={self.name}, addr=0x{self.address}, timeout={self.timeout_minutes}min, toalert={self.toalert_email})"