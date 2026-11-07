import cv2
import torch

print("环境就绪！")
img = cv2.imread("/home/miku/opencv/code/sys_test.jpg") 
if img is not None:
    cv2.imshow("Test", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()