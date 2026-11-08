#include <iostream>
#include <opencv2/opencv.hpp>
#include "auto_aim/light_bar_detector.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
        return -1;
    }

    cv::Mat img = cv::imread(argv[1]);
    if (img.empty()) {
        std::cerr << "Failed to load image: " << argv[1] << std::endl;
        return -1;
    }

    int max_width = 1280;
    if(img.cols > max_width){
        double scale = static_cast<double>(max_width) / img.cols;
        int new_height = static_cast<int>(img.rows * scale);
        cv::resize(img, img, cv::Size(max_width, new_height));
    }

    DetectorConfig cfg;
    cfg.enemy_color = "red"; 
    
    LightBarDetector detector(cfg);
    cv::imwrite("debug_mask.jpg", detector.buildColorMask(img));
    cv::Mat result = detector.detectAndDraw(img);

    cv::imshow("original", img);
    cv::imshow("result", result);
    cv::waitKey(0);
    return 0;
}