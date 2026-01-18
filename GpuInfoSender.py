import serial
import time
BAUD_RATE=9600
SERIAL_PORT = '/dev/ttyUSB0'
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

while True:
    with open("/sys/class/hwmon/hwmon1/temp1_input", 'r') as filename:
        value = int(int(filename.readline())/1000)
        ser.write(str(value).encode() + b'\n')
    time.sleep(0.1)
