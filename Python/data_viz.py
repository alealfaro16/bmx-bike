"""
PyTeapot module for drawing rotating cube using OpenGL as per
quaternion or yaw, pitch, roll angles received over serial port.
"""

import pygame
import math
from OpenGL.GL import *
from OpenGL.GLU import *
from pygame.locals import *
import numpy as np
import random
import time

import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style


import serial
ser = serial.Serial('/dev/tty.usbserial-AI05B84W', 38400) #change port name to yours

# Create arrays for plotting
time_arr = [] #store trials here (n)
roll_arr = [] 
speed_arr = [] 
accl_arr = [] 
i = 0


def main():
    video_flags = OPENGL | DOUBLEBUF
    pygame.init()
    screen = pygame.display.set_mode((800, 480), video_flags)
    pygame.display.set_caption("IMU orientation visualization")
    resizewin(700, 480)
    init()
    frames = 0
    i = 0
    start_ticks = pygame.time.get_ticks()

    #Enable streaming
    ser.write("stream;on;\n".encode())
    time.sleep(0.1)
    ser.write("balance;on;\n".encode())
    time.sleep(0.1)
    
    while 1:
        event = pygame.event.poll()
        if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
            break

        [yaw, pitch, roll, speed, accl] = read_data()
        time_arr.append((pygame.time.get_ticks()-start_ticks)/1000)
        roll_arr.append(roll)
        speed_arr.append(speed)
        accl_arr.append(accl)
        draw(1, yaw, pitch, roll, speed, accl)
        pygame.display.flip()
        frames += 1

    print("fps: %d" % ((frames*1000)/(pygame.time.get_ticks()-start_ticks)))
    ser.write("stop;\n".encode())
    time.sleep(0.2)
    ser.close()

    #Plot data collected
    fig = plt.figure()
    plt.subplot(3, 1, 1)
    plt.plot(time_arr, roll_arr, 'ko-')
    plt.title('time(s)')
    plt.ylabel('Roll angle (deg)')


    plt.subplot(3, 1, 2)
    plt.plot(time_arr, speed_arr, 'r.-')
    plt.xlabel('time (s)')
    plt.ylabel('Speed (RPM)')

    plt.subplot(3, 1, 3)
    plt.plot(time_arr, accl_arr, 'b.-')
    plt.xlabel('time (s)')
    plt.ylabel('Accl (delta RPM)')

    plt.show()


def resizewin(width, height):
    """
    For resizing window
    """
    if height == 0:
        height = 1
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45, 1.0*width/height, 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()


def init():
    glShadeModel(GL_SMOOTH)
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glClearDepth(1.0)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)


def cleanSerialBegin():
    try:
        line = ser.readline().decode('UTF-8').replace('\n', '')
        yaw = float(line.split('y')[1])
        pitch = float(line.split('p')[1])
        roll = float(line.split('r')[1])
        speed = int(line.split('s')[1])
        accl = int(line.split('a')[1])
    except Exception:
        pass


def read_data():
    ser.reset_input_buffer()
    cleanSerialBegin()
    line = ser.readline().decode('UTF-8').replace('\n', '')
    print(line)

    yaw = float(line.split('y')[1])
    pitch = float(line.split('p')[1])
    roll = float(line.split('r')[1])
    speed = int(line.split('s')[1])
    accl = int(line.split('a')[1])
    return [yaw, pitch, roll, speed, accl]


def draw(w, nx, ny, nz, speed, accl):
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    glTranslatef(0, 0.0, -7.0)

    drawText((-2.6, 1.8, 2), "PyTeapot", 18)
    drawText((-2.6, 1.6, 2), "Module to visualize quaternion or Euler angles data", 16)
    drawText((-2.6, -2, 2), "Press Escape to exit.", 16)

    yaw = nx
    pitch = ny
    roll = nz
    drawText((-2.6, -1.8, 2), "Yaw: %f, Pitch: %f, Roll: %f, Speed %d, Accl %d" %(yaw, pitch, roll, speed, accl), 16)
    glRotatef(-roll, 0.00, 0.00, 1.00)
    glRotatef(pitch, 1.00, 0.00, 0.00)
    glRotatef(yaw, 0.00, 1.00, 0.00)

    glBegin(GL_QUADS)
    glColor3f(0.0, 1.0, 0.0)
    glVertex3f(1.0, 0.2, -1.0)
    glVertex3f(-1.0, 0.2, -1.0)
    glVertex3f(-1.0, 0.2, 1.0)
    glVertex3f(1.0, 0.2, 1.0)

    glColor3f(1.0, 0.5, 0.0)
    glVertex3f(1.0, -0.2, 1.0)
    glVertex3f(-1.0, -0.2, 1.0)
    glVertex3f(-1.0, -0.2, -1.0)
    glVertex3f(1.0, -0.2, -1.0)

    glColor3f(1.0, 0.0, 0.0)
    glVertex3f(1.0, 0.2, 1.0)
    glVertex3f(-1.0, 0.2, 1.0)
    glVertex3f(-1.0, -0.2, 1.0)
    glVertex3f(1.0, -0.2, 1.0)

    glColor3f(1.0, 1.0, 0.0)
    glVertex3f(1.0, -0.2, -1.0)
    glVertex3f(-1.0, -0.2, -1.0)
    glVertex3f(-1.0, 0.2, -1.0)
    glVertex3f(1.0, 0.2, -1.0)

    glColor3f(0.0, 0.0, 1.0)
    glVertex3f(-1.0, 0.2, 1.0)
    glVertex3f(-1.0, 0.2, -1.0)
    glVertex3f(-1.0, -0.2, -1.0)
    glVertex3f(-1.0, -0.2, 1.0)

    glColor3f(1.0, 0.0, 1.0)
    glVertex3f(1.0, 0.2, -1.0)
    glVertex3f(1.0, 0.2, 1.0)
    glVertex3f(1.0, -0.2, 1.0)
    glVertex3f(1.0, -0.2, -1.0)
    glEnd()


def drawText(position, textString, size):
    font = pygame.font.SysFont("Courier", size, True)
    textSurface = font.render(textString, True, (255, 255, 255, 255), (0, 0, 0, 255))
    textData = pygame.image.tostring(textSurface, "RGBA", True)
    glRasterPos3d(*position)
    glDrawPixels(textSurface.get_width(), textSurface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, textData)


if __name__ == '__main__':
    main()
