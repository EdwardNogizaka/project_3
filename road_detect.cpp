//
//  main.cpp
//  Opencv
//
//  Created by Mr Edward Nguyen Quang Binh on 12/29/18.
//  Copyright © 2018 Mr Edward Nguyen Quang Binh. All rights reserved.
//
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "library.hpp"
//#include "libraries/segmentation.h"
//#include "libraries/colorConversion.h"

using namespace cv;
using namespace std;
RNG rng(12345);
int dilation_size = 5;
int k = 1960;
//int k = 7500;
//int k = 450;

int main(){
    Mat input_image;
    namedWindow("img",1);
    
    for(;;)
    {
        input_image = imread("image_40/" + to_string(k) + ".png");
//        input_image = imread("image_100/" + to_string(k) + ".png");
//        input_image = imread("Sign/" + to_string(k) + ".png");
//        input_image = imread("road.png");
        
        // Lane threshold
        int minThreshold[3] = {30, 10, 60};
        int maxThreshold[3] = {90, 20, 90};
        
        int minShadowTh[3] = {90, 43, 36};
        int maxShadowTh[3] = {120, 81, 171};
        int minLaneInShadow[3] = {90, 43, 97};
        int maxLaneInShadow[3] = {120, 80, 171};
        int binaryThreshold = 180;
        Mat imgThresholded, imgHSV, dst, dilation_dst;
        int shadowParam = 40;
        
        cvtColor(input_image, imgHSV, COLOR_BGR2HSV);
    //    cvtColor(src, imgHSV, COLOR_BGR2GRAY);
    //    threshold( imgHSV, imgThresholded, 180, 255, THRESH_BINARY);
        Mat image = imgHSV;
        Mat show_image;

        inRange(imgHSV, Scalar(minThreshold[0], minThreshold[1], minThreshold[2]),
                Scalar(maxThreshold[0], maxThreshold[1], maxThreshold[2]),
                imgThresholded);
        
//        Dilate follow erode
        Mat element = getStructuringElement(MORPH_RECT,
                                            Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                            Point( dilation_size, dilation_size ) );
        dilate( imgThresholded, imgThresholded, element );
        int erosion_size = 5;
        Mat element_erode = getStructuringElement( MORPH_RECT,
                                            Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                            Point( erosion_size, erosion_size ) );
        erode( imgThresholded, imgThresholded, element_erode);
        
//        ========================================================================
        Rect rec(0, 50, imgThresholded.cols, imgThresholded.rows - 50);
        vector<Vec4i> lines;

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        Canny( imgThresholded, imgThresholded, 0, 255, 3 );
        
        HoughLinesP(imgThresholded(rec), lines, 1, CV_PI/180, 50, 50, 10 );
        cvtColor(imgThresholded, show_image, COLOR_GRAY2BGR);

        for( size_t i = 0; i < lines.size(); i++ )
        {
            cout<<"1"<<endl;
            Vec4i l = lines[i];
            line(show_image(rec), Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
        }
        
        findContours( imgThresholded(rec), contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

        vector<vector<Point> > contours_poly( contours.size() );
        vector<Rect> boundRect( contours.size() );
        vector<Point2f>center( contours.size());
        vector<float>radius( contours.size() );
        
        
        for( int i = 0; i < contours.size(); i++)
        {
            approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
            boundRect[i] = boundingRect( Mat(contours_poly[i]) );
            minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
        }
        
        
        line(show_image, Point(0, 150), Point(show_image.cols, 150), Scalar(0,255,255), 1, CV_AA);
        line(show_image, Point(0, 200), Point(show_image.cols, 200), Scalar(0,255,255), 1, CV_AA);
        
        
        //    Mat drawing = Mat::zeros( input_image.size(), CV_8UC3);
        int max_area = 0;
        int max_index = 0;
        vector<Mat> rois(contours.size());
        Scalar color = Scalar( 0, 255, 0);
        for( int i = 0; i< contours.size(); i++)
        {
            Scalar color = Scalar( 0, 255, 0);
            int rec_width = boundRect[i].tl().x - boundRect[i].br().x;
            int rec_height = boundRect[i].tl().y - boundRect[i].br().y;
            
            //            imsiftwrite(to_string(k) + ".png", src(rec));
            if (max_area < rec_width*rec_height)
            {
                max_area = rec_width*rec_height;
                max_index = i;
            }
//            drawContours( show_image(rec), contours, i, color, 2, 8, hierarchy, 0, Point() );
//            rectangle( show_image(rec), boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
        
        }
        if (max_area != 0 ) rectangle( show_image(rec), boundRect[max_index].tl(), boundRect[max_index].br(), color, 2, 8, 0);
        imshow("img",input_image);
        imshow("keypoints", show_image);// Show blobs

        if(waitKey(30) == 32) k++;
        else if (waitKey(30) == 'a') k--;
    }
    return 0;
}
