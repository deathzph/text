#ifndef LIGHT_BAR_DETECTOR_H
#define LIGHT_BAR_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

struct DetectorConfig {
    std::string enemy_color = "red";
    int blue_h_min = 100, blue_h_max = 140;
    int blue_s_min = 90,  blue_v_min = 180;
    int red_h_min_1 = 0,   red_h_max_1 = 15;
    int red_h_min_2 = 165, red_h_max_2 = 180;
    int red_s_min = 50,    red_v_min = 220;
    int morph_kernel_size = 7;
    int morph_iterations = 2;
    double min_area      = 50.0;
    double max_area      =3000.0;
    double min_aspect    = 1.5;
    double max_aspect    = 10.0;
    double max_tilt_deg  = 45.0;
    bool save_result = true;
    std::string result_path = "result.jpg";
};

class LightBarDetector {
public:
    explicit LightBarDetector(const DetectorConfig& cfg = DetectorConfig());
    cv::Mat detectAndDraw(const cv::Mat& bgr);
      cv::Mat buildColorMask(const cv::Mat& bgr);
private:
    DetectorConfig cfg_;
    bool isLightBarContour(const std::vector<cv::Point>& contour,
                           const cv::RotatedRect& rect) const;
};

#endif // LIGHT_BAR_DETECTOR_H