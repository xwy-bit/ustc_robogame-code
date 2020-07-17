import cv2
import numpy as np


#hsv color range
#red1 range in h:0-10
red1low = np.array([0, 43, 64])
red1high = np.array([10, 255, 255])
#red2 range in h:156-180
red2low = np.array([156, 43, 64])
red2high = np.array([180, 255, 255])
#yellow range
yellowlow = np.array([11, 43, 64])
yellowhigh = np.array([34, 255, 255])
#green range
greenlow = np.array([35, 43, 64])
greenhigh = np.array([77, 255, 255])


#get the mask of competition area
#avoid the red target
def getArea(img):
    img = cv2.GaussianBlur(img, (5, 5), 0, 0)
    hsvimg = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    #get the mask of non red
    #get color area
    red1area = cv2.inRange(hsvimg, red1low, red1high)
    red2area = cv2.inRange(hsvimg, red2low, red2high)
    yellowarea = cv2.inRange(hsvimg, yellowlow, yellowhigh)
    greenarea = cv2.inRange(hsvimg, greenlow, greenhigh)

    #merge area
    colorarea = cv2.bitwise_or(red1area, red2area)
    colorarea = cv2.bitwise_or(colorarea, yellowarea)
    colorarea = cv2.bitwise_or(colorarea, greenarea)
    #kernel = np.ones((10,10),np.uint8)
    #maskarea = cv2.morphologyEx(colorarea, cv2.MORPH_OPEN, kernel)
    maskarea = cv2.bitwise_not(colorarea)
    #kernel = np.ones((10,10),np.uint8)
    #maskarea = cv2.morphologyEx(maskarea, cv2.MORPH_OPEN, kernel)
    #use cv2.RETR_EXTERNAL only find out contours
    maskarea, contours, hirrarchy = cv2.findContours(maskarea, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    mask = np.zeros((256, 256), np.uint8)
    mask = cv2.drawContours(mask, contours, -1, 255, -1)
    img = cv2.bitwise_and(img, img, mask = mask)
    return img


#extract red green yellow
def extcolor(img):
    img = getArea(img)
    #split the img
    #img = img[50:205,50:205:,:]
    #gauss blur
    img = cv2.GaussianBlur(img, (5, 5), 0, 0)

    #to hsv
    hsvimg = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

    #get color area
    red1area = cv2.inRange(hsvimg, red1low, red1high)
    red2area = cv2.inRange(hsvimg, red2low, red2high)
    yellowarea = cv2.inRange(hsvimg, yellowlow, yellowhigh)
    greenarea = cv2.inRange(hsvimg, greenlow, greenhigh)

    #merge area
    maskarea = cv2.bitwise_or(red1area, red2area)
    maskarea = cv2.bitwise_or(maskarea, yellowarea)
    maskarea = cv2.bitwise_or(maskarea, greenarea)

    #erode only to least the effect of green line
    kernel = np.ones((6, 6), dtype='uint8')
    maskarea = cv2.erode(maskarea, kernel, iterations=1)

    #add mask
    colorarea = cv2.bitwise_and(img, img, mask = maskarea)

    return maskarea, colorarea


#get object position using mask
def getPosition(mask):
    #get contours
    #only get outer contours
    mask, contours, hirrarchy = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    #sort contours with area
    contours = sorted(contours, key = (lambda contours: cv2.contourArea(contours)))
    #get contours moments
    maxcontours = []
    maxcontours.append(contours[-1])
    maxcontours.append(contours[-2])
    positions = []
    for contour in maxcontours:
        m = cv2.moments(contour)
        #get position x y
        try:
            cx = int(m['m10'] / m['m00'])
            cy = int(m['m01'] / m['m00'])
        except:
            continue
        positions.append([cx, cy])
    return positions


def getContourPosition(contour):
    m = cv2.moments(contour)
    try:
        cx = int(m['m10'] / m['m00'])
        cy = int(m['m01'] / m['m00'])
    except:
        return 0, 0
    return cx, cy


#sort color
def sortColor(colorarea):
    #get contours
    #only get outer contours
    #kernel = np.ones((3,3),np.uint8)
    #colorarea = cv2.erode(colorarea, kernel)
    mask = cv2.cvtColor(colorarea, cv2.COLOR_BGR2GRAY)
    mask, contours, hirrarchy = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    #sort contours with area
    maxcontours = []
    contours = sorted(contours, key = lambda contours:cv2.contourArea(contours))
    #get contours moments
    maxcontours.append(contours[-1])
    maxcontours.append(contours[-2])
    #sort in left area or right area
    #in left area, right to left is close to far
    #in right area, left to right is close to far
    maxcontours = sorted(maxcontours, key = (lambda maxcontours:getContourPosition(maxcontours)[1]))[::-1]
    hsvarea = cv2.cvtColor(colorarea, cv2.COLOR_BGR2HSV)
    sortedcolor = []
    for contour in maxcontours:
        sum = 0
        mask = np.zeros((colorarea.shape[0], colorarea.shape[1]), np.uint8)
        mask = cv2.drawContours(mask, [contour], -1, 255, -1)
        #plt.figure(figsize=(10, 10))
        #plt.imshow(mask)
        #plt.show()
        area = cv2.bitwise_and(hsvarea, hsvarea, mask=mask)
        h = area[:,:,0]
        v = area[:,:,2]
        nonzero_0, nonzero_1 = np.nonzero(v)
        #print(nonzero_0)
        for i in range(np.size(nonzero_0)):
            if h[nonzero_0[i], nonzero_1[i]] <= 10:
                sum += 180  #0-10 and 156-180 are both red. unify them.
            else:
                sum += h[nonzero_0[i], nonzero_1[i]]
        try:
            avr = sum / np.size(nonzero_0)
        except:
            avr = sum
        color = 0
        if avr>=0 and avr<=10:
            color = 1 #red
        if avr>=156 and avr<=180:
            color = 1 #red
        if avr>11 and avr<=34:
            color = 2 #yellow
        if avr>=35 and avr<=77:
            color = 3 #green
        sortedcolor.append(color)
    return sortedcolor