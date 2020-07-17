import cv2
import numpy as np

#yellow range
yellowlow = np.array([26, 43, 46])
yellowhigh = np.array([34, 255, 255])

def getOccupation(img, robotlr):
    #gauss blur
    img = cv2.GaussianBlur(img, (5, 5), 0, 0)
    #to hsv
    hsvimg = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    #get yellow area
    mask = cv2.inRange(hsvimg, yellowlow, yellowhigh)
    #erode
    kernel = np.ones((11, 11), dtype='uint8')
    mask = cv2.erode(mask, kernel, iterations=1)
    mask, contours, hirrarchy = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    contours = sorted(contours, key = (lambda contours: cv2.contourArea(contours)))
    #get occupied position
    m = cv2.moments(contours[-1])
    cx = int(m['m10'] / m['m00'])
    cy = int(m['m01'] / m['m00'])
    if cx <= 85:
        if robotlr == 0:
            pos = 1
        else:
            pos = 3
    elif cx>85 and cx<170:
        pos = 2
    else:
        if robotlr == 0:
            pos = 3
        else:
            pos = 1
    return mask, pos