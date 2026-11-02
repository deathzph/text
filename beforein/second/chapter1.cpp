#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;


//  import images
//int main(){
//    string path = "/home/miku/桌面/test.jpg";
//    Mat img = imread(path);
//    imshow("Image", img);
//    waitKey(0);
//    return 0;
//}

int main(){
     string path="/home/miku/桌面/test.jpg";
    VideoCapture cap(path);
    Mat img;
    while (true){
        cap.read(img);
        imshow("Video", img);
        waitKey(1);
    }
   
    
    return 0;
}