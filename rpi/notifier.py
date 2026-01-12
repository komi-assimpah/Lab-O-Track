import json
import urllib.request
import urllib.error
from datetime import datetime


class Notifier:
    def __init__(self, config):
        self.discord_webhook = "https://discord.com/api/webhooks/1457082272866898050/3tt51DZ-Wr58TC_fGOE853yjEbJ9oi661oCHvCj-GJ4qxwmX2uwGL-cck6vbcxe-7bsN"
        print(f"[Notifier] Discord: {'Configured' if self.discord_webhook else 'Not configured'}")

    
    # =========================================================================
    # DISCORD WEBHOOK
    # =========================================================================
    
    def _send_discord(self, device_id, device_name, event_type, message):
        """Send a notification to Discord via webhook"""
        if not self.discord_webhook:
            return False
        
        try:
            # Parse timestamp for date and time
            dt = datetime.now()
            date_str = dt.strftime("%Y-%m-%d")
            time_str = dt.strftime("%H:%M:%S")
            
            payload = {
                "content": f"**ALARM**\n**Device:** {device_name} ({device_id})\n**On:** {date_str}\n**At:** {time_str}\n**Message:** {message}"
            }
            data = json.dumps(payload).encode('utf-8')
            req = urllib.request.Request(
                self.discord_webhook, 
                data=data, 
                headers={
                    'Content-Type': 'application/json',
                    'User-Agent': 'LaboGateway/1.0'
                }
            )
            
            response = urllib.request.urlopen(req, timeout=10)
            if response.getcode() in [200, 204]:
                print(f"[Notifier] Discord notification sent")
                return True
            else:
                print(f"[Notifier] Discord notification failed, code: {response.getcode()}")
                return False
        except Exception as e:
            print(f"[Notifier] Discord notification failed: {e}")
            return False
    
    def notify(self, device_id, device_name, event_type, message):
        """
        Envoie une notification vers tous les canaux configurés
        
        Args:
            device_id: Identifiant de l'équipement (ex: "OSC-01")
            device_name: Nom lisible (ex: "Oscilloscope new gen")
            event_type: Type d'événement (ex: "ALARM_STARTED")
            message: Message descriptif
        """
        results = {
            "discord": False
        }
        
        # Envoyer via Discord
        if self.discord_webhook and event_type in ["ALARM_STARTED", "ALARM_STOPPED"]:
            results["discord"] = self._send_discord(device_id, device_name, event_type, message)
        
        return results