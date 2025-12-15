#!/usr/bin/env python3
"""
============================================================================
SMBUS MASTER - RASPBERRY PI
============================================================================

Script Python pour communiquer avec l'Arduino via I2C/SMBus.
Le Raspberry Pi agit comme maître et interroge l'Arduino périodiquement.

Installation :
    pip3 install smbus2

Configuration Raspberry Pi :
    1. Activer I2C : sudo raspi-config -> Interfacing Options -> I2C
    2. Vérifier : i2cdetect -y 1 (doit montrer 0x08)

Usage :
    python3 smbus_master.py
"""

import time
import struct
from smbus2 import SMBus

# ============================================================================
# CONSTANTES (doivent correspondre à protocol.h)
# ============================================================================

I2C_BUS = 1                    # Bus I2C du Raspberry Pi (1 pour modèles récents)
ARDUINO_ADDR = 0x08            # Adresse I2C de l'Arduino

# Commandes
CMD_GET_STATUS = 0x01
CMD_GET_TAG = 0x02
CMD_GET_TIMER = 0x03

# Codes de statut
STATUS_OK = 0x00
STATUS_TAG_PRESENT = 0x01
STATUS_TAG_MISSING = 0x02
STATUS_ALERT = 0x03

# Noms lisibles pour les statuts
STATUS_NAMES = {
    STATUS_OK: "OK - Équipement en place",
    STATUS_TAG_PRESENT: "TAG PRÉSENT - En utilisation",
    STATUS_TAG_MISSING: "TAG ABSENT - Timer en cours",
    STATUS_ALERT: "⚠️ ALERTE - Timer expiré!"
}


# ============================================================================
# CLASSE PRINCIPALE
# ============================================================================

class LabTrackMaster:
    """
    Classe pour communiquer avec l'Arduino Lab-O-Track via I2C.
    """
    
    def __init__(self, bus_id: int = I2C_BUS, address: int = ARDUINO_ADDR):
        """
        Initialise la connexion I2C.
        
        Args:
            bus_id: Numéro du bus I2C (1 par défaut sur RPi récents)
            address: Adresse I2C de l'Arduino (0x08 par défaut)
        """
        self.bus_id = bus_id
        self.address = address
        self.bus = None
        
    def connect(self) -> bool:
        """
        Ouvre la connexion I2C.
        
        Returns:
            True si connexion réussie, False sinon
        """
        try:
            self.bus = SMBus(self.bus_id)
            print(f"✓ Connecté au bus I2C {self.bus_id}")
            return True
        except Exception as e:
            print(f"✗ Erreur connexion I2C: {e}")
            return False
    
    def disconnect(self):
        """Ferme la connexion I2C."""
        if self.bus:
            self.bus.close()
            self.bus = None
            print("✓ Déconnecté du bus I2C")
    
    def get_status(self) -> int:
        """
        Demande le statut actuel à l'Arduino.
        
        Returns:
            Code de statut (STATUS_OK, STATUS_TAG_PRESENT, etc.)
        """
        try:
            # Envoyer la commande puis lire la réponse
            self.bus.write_byte(self.address, CMD_GET_STATUS)
            time.sleep(0.01)  # Petit délai pour laisser l'Arduino répondre
            status = self.bus.read_byte(self.address)
            return status
        except Exception as e:
            print(f"✗ Erreur get_status: {e}")
            return -1
    
    def get_tag_id(self) -> str:
        """
        Demande l'ID du tag RFID actuel.
        
        Returns:
            ID du tag en ASCII (10 caractères) ou None si erreur
        """
        try:
            # Envoyer la commande
            self.bus.write_byte(self.address, CMD_GET_TAG)
            time.sleep(0.01)
            
            # Lire 11 bytes (10 caractères + null)
            data = self.bus.read_i2c_block_data(self.address, CMD_GET_TAG, 11)
            
            # Convertir en string (ignorer le null terminator)
            tag_bytes = bytes(data[:10])
            tag_str = tag_bytes.decode('ascii', errors='ignore').strip('\x00')
            return tag_str
        except Exception as e:
            print(f"✗ Erreur get_tag_id: {e}")
            return None
    
    def get_timer(self) -> int:
        """
        Demande le temps restant avant alerte.
        
        Returns:
            Temps en secondes, ou -1 si erreur
        """
        try:
            # Envoyer la commande
            self.bus.write_byte(self.address, CMD_GET_TIMER)
            time.sleep(0.01)
            
            # Lire 2 bytes (big-endian)
            data = self.bus.read_i2c_block_data(self.address, CMD_GET_TIMER, 2)
            timer = (data[0] << 8) | data[1]
            return timer
        except Exception as e:
            print(f"✗ Erreur get_timer: {e}")
            return -1
    
    def poll(self) -> dict:
        """
        Interroge l'Arduino et retourne toutes les informations.
        
        Returns:
            Dictionnaire avec status, tag, timer
        """
        result = {
            'status': None,
            'status_name': 'Erreur',
            'tag': None,
            'timer': None
        }
        
        # Récupérer le statut
        status = self.get_status()
        if status >= 0:
            result['status'] = status
            result['status_name'] = STATUS_NAMES.get(status, f'Inconnu ({status})')
            
            # Si tag présent ou alerte, récupérer les détails
            if status in (STATUS_TAG_PRESENT, STATUS_TAG_MISSING, STATUS_ALERT):
                result['tag'] = self.get_tag_id()
                result['timer'] = self.get_timer()
        
        return result


# ============================================================================
# PROGRAMME PRINCIPAL
# ============================================================================

def main():
    """Boucle principale de polling."""
    
    print("=" * 60)
    print("LAB-O-TRACK - SMBus Master (Raspberry Pi)")
    print("=" * 60)
    print()
    
    # Créer l'instance
    master = LabTrackMaster()
    
    # Connexion
    if not master.connect():
        print("Impossible de se connecter. Vérifiez :")
        print("  1. I2C activé (sudo raspi-config)")
        print("  2. Arduino connecté et flashé")
        print("  3. Câblage correct (SDA, SCL, GND)")
        return
    
    try:
        print("\nDémarrage du polling (Ctrl+C pour arrêter)...")
        print("-" * 60)
        
        poll_count = 0
        
        while True:
            poll_count += 1
            
            # Interroger l'Arduino
            data = master.poll()
            
            # Afficher les résultats
            timestamp = time.strftime("%H:%M:%S")
            print(f"\n[{timestamp}] Poll #{poll_count}")
            print(f"  Status : {data['status_name']}")
            
            if data['tag']:
                print(f"  Tag    : {data['tag']}")
            
            if data['timer'] is not None and data['timer'] >= 0:
                minutes = data['timer'] // 60
                seconds = data['timer'] % 60
                print(f"  Timer  : {minutes}m {seconds}s")
            
            # Alerte spéciale
            if data['status'] == STATUS_ALERT:
                print("\n  *** ALERTE DÉCLENCHÉE ! ***")
                print(f"  Équipement {data['tag']} manquant depuis trop longtemps!")
            
            # Attendre avant le prochain poll
            time.sleep(2)
            
    except KeyboardInterrupt:
        print("\n\nArrêt demandé par l'utilisateur")
    
    finally:
        master.disconnect()
        print("Terminé.")


if __name__ == "__main__":
    main()
