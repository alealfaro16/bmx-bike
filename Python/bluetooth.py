import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
import numpy as np
import random
import serial
import time

#initialize serial port
ser = serial.Serial()
ser.port = '/dev/ttyUSB0' #Arduino serial port
ser.baudrate = 38400
ser.timeout = 10 #specify timeout when using readline()
ser.open()

if ser.is_open==True:
	print("\nAll right, serial port now open. Configuration:\n")
	print(ser, "\n") #print serial parameters

ser.flushInput() #This gives the bluetooth a little kick

# Create figure for plotting
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
xs = [] #store trials here (n)
ys = [] #store relative frequency here
i = 0

# This function is called periodically from FuncAnimation
def animate(i, xs, ys):

		#Aquire and parse data from serial port
		line=ser.readline()      #ascii
		#line_as_list = line.split(b',')
		#int(line_as_list[0])
		#encoder_count = line_as_list[1]
		encoder_count_list = line.split(b'\n')
		encoder_count_float = float(encoder_count_list[0])

		# Add x and y to lists
		xs.append(i)
		ys.append(encoder_count_float)
		i = i + 1

		# Limit x and y lists to 20 items
		#xs = xs[-20:]
		#ys = ys[-20:]

		# Draw x and y lists
		ax.clear()
		ax.plot(xs, ys, label="Encoder Count")
		#ax.plot(xs, rs, label="Theoretical Probability")

		# Format plot
		plt.xticks(rotation=45, ha='right')
		plt.subplots_adjust(bottom=0.30)
		plt.title('Encoder count live plotting')
		plt.ylabel('Encoder Count')
		plt.xlabel('Time (s)')
		plt.legend()
		plt.axis([0, None, 0, 1]) #Use for arbitrary number of trials
		#plt.axis([1, 100, 0, 1.1]) #Use for 100 trial demo

# Set up plot to call animate() function periodically
ani = animation.FuncAnimation(fig, animate, fargs=(xs,ys), interval=100)
plt.show()
