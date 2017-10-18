
from machine import UART
from network import bluetooth
import machine
import os

uart = UART(0, baudrate=115200)
os.dupterm(uart)
pycom.wifi_on_boot(False)
bluetooth.deinit()
machine.main('main.py')
