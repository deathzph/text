#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    cout << "hello,world" << endl;
    Mat img1 = imread("/home/miku/图片/Warframe0000.png", 1);
    cv::imshow("img", img1);
    while(waitKey(0) != 's');
    return 0;
}
