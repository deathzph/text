#include <iostream>
#include <opencv2/opencv.hpp>

int main(){
    std::cout << "Hello, OpenCV!" << std::endl;
    
    cv::Mat img(200,200, CV_8UC3, cv::Scalar(0,0,255));
    std::cout << "图像尺寸: " << img.cols << "x" << img.rows << std::endl;
}