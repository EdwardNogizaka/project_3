//
//  cross_detect.cpp
//  Opencv
//
//  Created by Mr Edward Nguyen Quang Binh on 12/29/18.
//  Copyright © 2018 Mr Edward Nguyen Quang Binh. All rights reserved.
//
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int main(){
    Mat input_image;
    Mat imgThresholded, imgHSV, dst, dilation_dst;
    Mat show_image;

    namedWindow("img",1);
    
    for(;;)
    {
//      ================================= Convert HSV and threshold the road ====================================
        input_image = imread("image_40/" + to_string(k) + ".png");
//        input_image = imread("463.png");

        int minThreshold[3] = {30, 10, 60};
        int maxThreshold[3] = {90, 20, 90};

        cvtColor(input_image, imgHSV, COLOR_BGR2HSV);
        Mat image = imgHSV;

        inRange(imgHSV, Scalar(minThreshold[0], minThreshold[1], minThreshold[2]),
                Scalar(maxThreshold[0], maxThreshold[1], maxThreshold[2]),
                imgThresholded);
        
//      =========================================================================================================

        
        
//      ================================= Dilate follow erode ===================================================

        int erosion_size = 5;
        int dilation_size = 5;

        Mat element = getStructuringElement(MORPH_RECT,
                                            Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                            Point( dilation_size, dilation_size ) );
        Mat element_erode = getStructuringElement( MORPH_RECT,
                                                  Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                                  Point( erosion_size, erosion_size ) );
        dilate( imgThresholded, imgThresholded, element );
        erode( imgThresholded, imgThresholded, element_erode);
        
//      ============================================================================================================
        
        
        
//      ======================================== York and cross detection ==========================================
        vector<Vec4i> lines;

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        int morph_size = 1;
        
        Mat img_check = imgThresholded.clone();
        
        cvtColor(imgThresholded, show_image, COLOR_GRAY2BGR);
        
        Point ptl(0, 150);
        Point pbr(show_image.cols, 200);
        
        line(show_image, Point(0, 150), Point(show_image.cols, 150), Scalar(0,255,0), 1, CV_AA);
        line(show_image, Point(0, 200), Point(show_image.cols, 200), Scalar(0,255,0), 1, CV_AA);
        
        Rect rec_1(ptl, pbr);
        threshold(img_check(rec_1), img_check(rec_1), 0, 255, THRESH_BINARY_INV);
        imshow("Check", img_check(rec_1));
        
        if (countNonZero(img_check(rec_1)) == 0) cout<<"York - Cross detected"<<endl;
//      ===========================================================================================================

        imshow("Threshold image", show_image);
        
        waitKey(1);
    }
    return 0;
}
