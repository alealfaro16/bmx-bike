import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
import numpy as np
import random
import serial
import time

#initialize serial port
ser = serial.Serial()
ser.port = '/dev/tty.usbserial-AI05B84W' #Arduino serial port
ser.baudrate = 38400
ser.timeout = 10 #specify timeout when using readline()
ser.open()

if ser.is_open==True:
	print("\nAll right, serial port now open. Configuration:\n")
	print(ser, "\n") #print serial parameters

ser.flushInput() #This gives the bluetooth a little kick

#Enable streaming
ser.write("stream;on;\n".encode())
time.sleep(0.1)
ser.write("balance;on;\n".encode())

# Create figure for plotting
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
xs = [] #store trials here (n)
ys = [] #store relative frequency here
rs = [] #store relative frequency here
i = 0

def cleanSerialBegin():
    try:
        line = ser.readline().decode('UTF-8').replace('\n', '')
        yaw = float(line.split('y')[1])
        pitch = float(line.split('p')[1])
        roll = float(line.split('r')[1])
        speed = int(line.split('s')[1])
    except Exception:
        pass

# This function is called periodically from FuncAnimation
def animate(i, xs, ys, rs):

		#Aquire and parse data from serial port
		ser.reset_input_buffer()
		cleanSerialBegin()
		line = ser.readline().decode('UTF-8').replace('\n', '')
		print(line)

		yaw = float(line.split('y')[1])
		pitch = float(line.split('p')[1])
		roll = float(line.split('r')[1])
		speed = int(line.split('s')[1])

		# Add x and y to lists
		xs.append(i)
		ys.append(roll)
		rs.append(speed)
		i = i + 1

		# Limit x and y lists to 20 items
		#xs = xs[-20:]
		#ys = ys[-20:]

		# Draw x and y lists
		ax.clear()
		ax.plot(xs, ys, label="Roll")
		//ax.plot(xs, rs, label="Theoretical Probability")

		# Format plot
		plt.xticks(rotation=45, ha='right')
		plt.subplots_adjust(bottom=0.30)
		plt.title('Encoder count live plotting')
		plt.ylabel('Encoder Count')
		plt.xlabel('Time (s)')
		plt.legend()
		plt.axis([0, None, -5, 5]) #Use for arbitrary number of trials
		#plt.axis([1, 100, 0, 1.1]) #Use for 100 trial demo

# Set up plot to call animate() function periodically
ani = animation.FuncAnimation(fig, animate, fargs=(xs,ys,rs), interval=100)
plt.show()
