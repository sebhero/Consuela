import cv2
import numpy as np
import serial
import time

##list_intensities = []
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=0)

cap = cv2.VideoCapture(1)
print "Starting video-capture"

_, backframe = cap.read()
before_img = backframe
height, width, _ = backframe.shape
centerX = width/2
centerY = height/2
center_distance = 30 ##distance in cm

cx = -1
cy = -1
distance_x = 0
distance_y = 0
angle_top = 0
angle_far = 0
sum_far = 0
height_subtractor = 0
print "Height: " + str(height)
print "Width: " + str(width)
print "centerX: " + str(centerX)
print "centerY: " + str(centerY)
while True:
    _, frame = cap.read()
    readline = ser.readline()
    ##readline = ser.readline()
    after_img = frame

    ##bf_before = cv2.bilateralFilter(backframe, 9, 150, 150)
    ##bf_after = cv2.bilateralFilter(frame, 9, 150, 150)


    ##gray_beforeBil = cv2.cvtColor(after_img, cv2.COLOR_BGR2GRAY)
    gray_before = cv2.cvtColor(before_img, cv2.COLOR_BGR2GRAY)
    gray_after = cv2.cvtColor(after_img, cv2.COLOR_BGR2GRAY)

    imgsub = cv2.absdiff(gray_before, gray_after)

    ##cv2.imshow('graybefore', gray_before)
    ##cv2.imshow('beforebil', gray_beforeBil)
    cv2.imshow('grayafter', gray_after)
    cv2.imshow('imgsub', imgsub)
    kernel = np.ones((5,5),np.uint8)
    close_operated_image = cv2.morphologyEx(imgsub, cv2.MORPH_CLOSE, kernel)
    _, thresholded = cv2.threshold(close_operated_image, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
    
    median = cv2.medianBlur(thresholded, 7)
    
    cv2.imshow('median', median)
    _, contours, _ = cv2.findContours(median, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    for i in range(len(contours)):
        cimg = np.zeros_like(imgsub)
        if len(contours)<40:
            cv2.drawContours(cimg, contours, -1, (100, 0, 255),2)
            break
        ##pts = np.where(cimg == 255)
        ##list_intensities.append(imgsub[pts[0],pts[1]])
    ##cnt = contours[0]
    ##M = cv2.moments(cnt)
    ##area = cv2.contourArea(cnt)
    for i in range(len(contours)):
        cnt = contours[i]
        M = cv2.moments(cnt)
        area = cv2.contourArea(cnt)
        
        if area > 4000 and M['m00'] != 0.0 and len(contours)<40:
            cx = int(M['m10']/M['m00'])
            cy = int(M['m01']/M['m00'])
            cxstr = "CX: " + str(cx)
            cystr = "CY: " + str(cy)
            ##print cxstr
            ##print cystr
            cv2.circle(cimg,(cx,cy), 5, (255,255,255), -1)
            
            break
        
        ##print M['m00']
        
    ##cv2.imshow('cimg',cimg)
    if cx != -1 and cy != -1:
        
        if cx < centerX - 42:
            ser.write(b'L\n')
            ##ser.write(b'Left\n')
        elif cx > centerX + 42:
            ser.write(b'R\n')
            ##ser.write(b'Right\n')
        elif centerX - 45 <= cx <= centerX + 45:
            angle_top = 27 - (cy * 0.0563)
            ##print "Angle_top: " + str(angle_top)
            angle_far = 180 - (143 + angle_top)
            ##print "Angle_far: " + str(angle_far)
            sum_far = 25 / (np.sin(np.deg2rad(angle_far)))
            height_subtractor = (2.25 * (np.sin(np.deg2rad(180-(90 + angle_far)))))/(np.sin(np.deg2rad(angle_far)))
            ##print "Height subtractor: " + str(height_subtractor)
            distance_y = 29 + (sum_far*(np.sin(np.deg2rad(angle_top)))) - height_subtractor
            ser.write(str(distance_y))
    cx = -1
    if readline != '':
        print readline + "\n"
    ##if readline != '':
        ##print readline + "\n"
    ##cv2.imshow('imgsub', imgsub)
    if cv2.waitKey(1) & 0xFF == ord('q'): 
        break

cv2.destroyAllWindows()
                           
