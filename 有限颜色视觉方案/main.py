from imgtrans import *
from hospitalimg import *
'''
from uart import *
'''
import cv2
import numpy as np

#robot in left area or right area
#left 0
#right 1
robotlr = 0

#start vision

if __name__ == '__main__':
    img = cv2.imread('D:/robogame/robogamecv/raspivision/images/text (16).jpg')
    img = cv2.resize(img, (256, 256))
    mask, color = extcolor(img)
    s = sortColor(color)
    print(s)
    #color, pos = getOccupation(img, robotlr)
    #print(pos)
    cv2.imshow(' ',color)
    cv2.waitKey(0)
    cv2.destroyAllWindows()