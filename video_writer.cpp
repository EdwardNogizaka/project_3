//
//  video.cpp
//  
//
//  Created by Mr Edward Nguyen Quang Binh on 1/6/19.
//
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "library.hpp"
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio.hpp>  // Video write

using namespace cv;
using namespace std;

int main()
{
    VideoWriter writer;
    int i=430;
    Mat src = imread("image_100/"+to_string(i) + ".png");
    imshow("img", src);
    string filename = "out.avi";
    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');  // select desired codec (must be available at runtime)

    int fps = 60;
    writer.open(filename, codec, fps, src.size(), true);
    cout<<"1"<<endl;
    for(int i=300;i<455;i++)
    {
        src = imread("image_100/" + to_string(i) + ".png");
        writer.write(src);
    }
    cout<<"Done"<<endl;
    return 1;
    
    
    
}


