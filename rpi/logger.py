import datetime
import json
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

class Logger:
    def __init__(self, log_file="data/events.json"):
        self.log_file = os.path.join(SCRIPT_DIR, log_file)
        self._ensure_file_exists()

    def _ensure_file_exists(self):
        if not os.path.exists(self.log_file) or os.path.getsize(self.log_file) == 0:
            with open(self.log_file, "w") as f:
                json.dump([], f)

    def log(self, event_type, device_id, device_name, message):
        timestamp = datetime.datetime.now().isoformat()
        
        event = {
            "timestamp": timestamp,
            "event_type": event_type,
            "device_id": device_id,
            "device_name": device_name,
            "message": message
        }
        
        print(f"[{timestamp}] {event_type}: {device_name} - {message}")
        
        try:
            with open(self.log_file, "r") as f:
                events = json.load(f)
        except (json.JSONDecodeError, FileNotFoundError):
            events = []
        
        events.append(event)
        with open(self.log_file, "w") as f:
            json.dump(events, f, indent=2)