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
    Mat img_gradient;

    
    int morph_size = 1;
    int erosion_size = 5;
    int dilation_size = 5;
    int minThreshold[3] = {30, 10, 60};
    int maxThreshold[3] = {90, 20, 90};
    
    
    namedWindow("image", 1);
    for(;;)
    {
//      ================================= Convert HSV and threshold the road ====================================
//        input_image = imread("image_40/" + to_string(k) + ".png");
        input_image = imread("1964.png");
        
        cvtColor(input_image, imgHSV, COLOR_BGR2HSV);
        Mat image = imgHSV;

        inRange(imgHSV, Scalar(minThreshold[0], minThreshold[1], minThreshold[2]),
                Scalar(maxThreshold[0], maxThreshold[1], maxThreshold[2]),
                imgThresholded);
        
//      =========================================================================================================

        
        
//      ================================= Dilate follow erode ===================================================

        Mat element = getStructuringElement(MORPH_RECT,
                                            Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                            Point( dilation_size, dilation_size ) );
        Mat element_erode = getStructuringElement( MORPH_RECT,
                                                  Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                                  Point( erosion_size, erosion_size ) );
        dilate( imgThresholded, imgThresholded, element );
        erode( imgThresholded, imgThresholded, element_erode);
        
//      ===========================================================================================================
        
        
//      ======================================== Dilate - erode ===================================================
        Mat element_grap = getStructuringElement( 0, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
        morphologyEx( imgThresholded, img_gradient, MORPH_GRADIENT, element_grap );
//      ===========================================================================================================
        
        imshow("Threshold image", img_gradient);
        waitKey(1);
    }
    return 0;
}
