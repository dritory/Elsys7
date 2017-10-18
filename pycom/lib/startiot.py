from network import LoRa
import socket
import time
import binascii
import pycom
import machine

class Startiot:

    def __init__(self):

    def connect(self, blocking, dev_eui,app_eui,app_key):

        self.dev_eui = binascii.unhexlify(app_eui)
        self.app_eui = binascii.unhexlify(app_eui)
        self.app_key = binascii.unhexlify(app_key)

        self.lora = LoRa(mode=LoRa.LORAWAN)
        self.lora.join(activation=LoRa.OTAA, auth=(self.dev_eui, self.app_eui, self.app_key), timeout=0)

        while not self.lora.has_joined():
            pass

        self.s = socket.socket(socket.AF_LORA, socket.SOCK_RAW)

        # set the LoRaWAN data rate
        self.s.setsockopt(socket.SOL_LORA, socket.SO_DR, 5)

        # make the socket non-blocking
        self.s.setblocking(blocking)

    def send(self, data):
        self.s.send(data)

    def recv(self, length):
        return self.s.recv(length)
