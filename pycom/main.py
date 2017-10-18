from startiot import Startiot
import pycom
from time import sleep
from machine import Pin
from onewire import DS18X20
from onewire import OneWire

dev_eui = binascii.unhexlify("000000000000034d")
app_eui = binascii.unhexlify("00000000000000ee")
app_key = binascii.unhexlify("77504d7273735173567262527171324b")


pycom.heartbeat(False)
iot = Startiot()
iot.connect(False,dev_eui,app_eui,app_key)

ow = OneWire(Pin('P23'))
temp = DS18X20(ow)

state = False

i = 0
while True:
    tmp = temp.read_temp_async()
    print(tmp)
    sleep(1)
    temp.start_convertion()

    #if(p_in() == 1):
    iot.send("TEMP,%f" % tmp)
    #    i++
    sleep(10)
