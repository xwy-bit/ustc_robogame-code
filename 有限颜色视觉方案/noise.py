import cv2
import numpy as np

def clamp(pv):
    if pv > 255:
        return 255
    elif pv < 0:
        return 0
    else:
        return pv
 
 
def gaussian_noise_demo(image):
    h, w, c = image.shape
    for row in range(0, h):
        for col in range(0, w):
            s = np.random.normal(0, 15, 3)
            b = image[row, col, 0]  # blue
            g = image[row, col, 1]  # green
            r = image[row, col, 2]  # red
            image[row, col, 0] = clamp(b + s[0])
            image[row, col, 1] = clamp(g + s[1])
            image[row, col, 2] = clamp(r + s[2])
    return image

if __name__ == '__main__':
    img = cv2.imread('./images/test3.jpg')
    img = gaussian_noise_demo(img)
    cv2.imwrite('./images/test4.jpg', img)
    print('finish')