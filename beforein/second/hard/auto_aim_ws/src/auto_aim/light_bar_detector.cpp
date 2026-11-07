#include "auto_aim/light_bar_detector.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <opencv2/geometry.hpp>

LightBarDetector::LightBarDetector(const DetectorConfig& cfg) : cfg_(cfg) {}


cv::Mat LightBarDetector::buildColorMask(const cv::Mat& bgr) {
    cv::Mat hsv;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);

    cv::Mat mask;
    if (cfg_.enemy_color == "blue") {
        cv::inRange(hsv,
                    cv::Scalar(cfg_.blue_h_min, cfg_.blue_s_min, cfg_.blue_v_min),
                    cv::Scalar(cfg_.blue_h_max, 255, 255),
                    mask);
    } else {
        cv::Mat mask1, mask2;
        cv::inRange(hsv, cv::Scalar(cfg_.red_h_min_1, cfg_.red_s_min, cfg_.red_v_min),
                    cv::Scalar(cfg_.red_h_max_1, 255, 255), mask1);
        cv::inRange(hsv, cv::Scalar(cfg_.red_h_min_2, cfg_.red_s_min, cfg_.red_v_min),
                    cv::Scalar(cfg_.red_h_max_2, 255, 255), mask2);
        cv::bitwise_or(mask1, mask2, mask);
    }

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,
        cv::Size(cfg_.morph_kernel_size, cfg_.morph_kernel_size));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel,
                     cv::Point(-1, -1), cfg_.morph_iterations);
    return mask;
}

bool LightBarDetector::isLightBarContour(
    const std::vector<cv::Point>& contour, const cv::RotatedRect& rect) const {

    double area = cv::contourArea(contour);
    if (area < cfg_.min_area || area > cfg_.max_area) return false;

    double w = rect.size.width;
    double h = rect.size.height;
    double long_side  = std::max(w, h);
    double short_side = std::min(w, h);

    if (short_side < 1e-6) return false;
    double aspect = long_side / short_side;
    if (aspect < cfg_.min_aspect || aspect > cfg_.max_aspect) return false;

    return true;
}

cv::Mat LightBarDetector::detectAndDraw(const cv::Mat& bgr) {
    cv::Mat result = bgr.clone();
    cv::Mat mask = buildColorMask(bgr);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours) {
        cv::RotatedRect rect = cv::minAreaRect(contour);
        if (!isLightBarContour(contour, rect)) continue;

        cv::Point2f vertices[4];
        rect.points(vertices);
        for (int i = 0; i < 4; ++i) {
            cv::line(result, vertices[i], vertices[(i + 1) % 4],
                     cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
        }
    }

    if (cfg_.save_result) {
        cv::imwrite(cfg_.result_path, result);
        std::cout << "[INFO] result saved to: " << cfg_.result_path << std::endl;
    }
    return result;
}