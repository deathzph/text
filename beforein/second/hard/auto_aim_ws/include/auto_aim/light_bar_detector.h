#ifndef LIGHT_BAR_DETECTOR_H
#define LIGHT_BAR_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

struct DetectorConfig {
    std::string enemy_color = "blue";
    int blue_h_min = 100, blue_h_max = 140;
    int blue_s_min = 90,  blue_v_min = 180;
    int red_h_min_1 = 0,   red_h_max_1 = 12;
    int red_h_min_2 = 168, red_h_max_2 = 180;
    int red_s_min = 60,    red_v_min = 60;
    int morph_kernel_size = 3;
    int morph_iterations = 2;
    double min_area      = 100.0;
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

private:
    DetectorConfig cfg_;
    cv::Mat buildColorMask(const cv::Mat& bgr);
    // 这里的 std::vector 必须带 <cv::Point> 模板参数
    bool isLightBarContour(const std::vector<cv::Point>& contour,
                           const cv::RotatedRect& rect) const;
};

#endif // LIGHT_BAR_DETECTOR_H