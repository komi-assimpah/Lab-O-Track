import time
from arduino_device import STATUS_TAG_PRESENT, STATUS_TIMER_RUNNING, STATUS_ALARM_ACTIVE

class Gateway:
    def __init__(self, i2c_master, arduino_devices, logger):
        self.i2c = i2c_master
        self.arduino_devices = arduino_devices
        self.logger = logger
        self.previous_states = {}

    def poll_all(self):
        for device in self.arduino_devices:
            device.poll(self.i2c)
            
    def _log_state_change(self, device, event_type, message):
        self.logger.log(event_type, device.id, device.name, message)
            
    def check_state_changes(self):
        """Check for state changes and log them"""
        for device in self.arduino_devices:
            current_status = device.last_status
            previous_status = self.previous_states.get(device.id, None)
            
            if previous_status is None:# First poll
                self.previous_states[device.id] = current_status
                continue
            
            if current_status != previous_status:
                # Object returned (was not present, now present)
                if not (previous_status & STATUS_TAG_PRESENT) and (current_status & STATUS_TAG_PRESENT):
                    self._log_state_change(device, "OBJECT_RETURNED", "Objet returned to its place")
                
                # Object removed (was present, now not)
                if (previous_status & STATUS_TAG_PRESENT) and not (current_status & STATUS_TAG_PRESENT):
                    self._log_state_change(device, "OBJECT_REMOVED", "Objet removed from its place")
                
                # Alarm started
                if not (previous_status & STATUS_ALARM_ACTIVE) and (current_status & STATUS_ALARM_ACTIVE):
                    self._log_state_change(device, "ALARM_STARTED", "Alarme started")
                    # TODO: send email alert
                
                # Alarm stopped
                if (previous_status & STATUS_ALARM_ACTIVE) and not (current_status & STATUS_ALARM_ACTIVE):
                    self._log_state_change(device, "ALARM_STOPPED", "Alarme stopped")
                
                self.previous_states[device.id] = current_status
    
    def run(self, poll_interval_seconds=5):
        print("Gateway running...")
        self.logger.log("SYSTEM", "GATEWAY", "Gateway", "Gateway running")
        while True:
            self.poll_all()
            self.check_state_changes()
            time.sleep(poll_interval_seconds)