import serial.tools.list_ports
import time

def after_upload(source, target, env):
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        if "USB" in p.description and "modem" in p.device.lower():
            try:
                s = serial.Serial(p.device, 1200)
                s.setDTR(False)
                time.sleep(0.1)
                s.setDTR(True)
                s.close()
                print("Sent USB reset toggle.")
                break
            except Exception as e:
                print("Failed to reset:", e)

Import("env")
env.AddPostAction("upload", after_upload)
