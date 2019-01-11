

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "library.hpp"

//#include "detectlane.h"

//bool STREAM = true;

//DetectLane *detect;

int skipFrame = 1;
using namespace cv;
bool first = false;
int i = 0;

int minThreshold[3] = {80, 244, 181};
int maxThreshold[3] = {105, 255, 205};
int minShadowTh[3] = {90, 43, 36};
int maxShadowTh[3] = {120, 81, 171};
int minLaneInShadow[3] = {90, 43, 97};
int maxLaneInShadow[3] = {120, 80, 171};
int shadowParam = 40;

int binaryThreshold = 180;
using namespace std;
RNG rng(12345);




int main(int argc, char **argv)
{
    cvCreateTrackbar("LowH", "Threshold", &minThreshold[0], 179);
    cvCreateTrackbar("HighH", "Threshold", &maxThreshold[0], 179);
    cvCreateTrackbar("LowS", "Threshold", &minThreshold[1], 255);
    cvCreateTrackbar("HighS", "Threshold", &maxThreshold[1], 255);
    cvCreateTrackbar("LowV", "Threshold", &minThreshold[2], 255);
    cvCreateTrackbar("HighV", "Threshold", &maxThreshold[2], 255);
    cvCreateTrackbar("Shadow Param", "Threshold", &shadowParam, 255);
    
    cv::namedWindow("View");
    cv::namedWindow("Threshold");
    
    Mat src;
    Mat imgThresholded, imgHSV, dst;
    Mat roi;
    int k=1960;
    while(1)
    {
        src = imread("image_40/" + to_string(k) + ".png");
        
        
        
        cvtColor(src, imgHSV, COLOR_BGR2HSV);
        Mat image = imgHSV.clone();
        
        //    int x = 1, y = 1;
        //    Vec3b color = image.at<Vec3b>(Point(x,y));
        //    cout<<color<<endl;
        
        //    imshow("Binary", imgThresholded);
        //    VideoWriter writer;
        //    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
        //    double fps = 25.0;
        //    string filename = "live.avi";
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        
        inRange(imgHSV, Scalar(minThreshold[0], minThreshold[1], minThreshold[2]),
                Scalar(maxThreshold[0], maxThreshold[1], maxThreshold[2]),
                imgThresholded);
        
        //    filter_image(imgThresholded, imgThresholded);
        cv::Mat struct_elt = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(8, 8));
        cv::dilate(imgThresholded, imgThresholded, struct_elt);
        
        findContours( imgThresholded, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
        
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
        
        //    Mat drawing = Mat::zeros( input_image.size(), CV_8UC3);
        vector<Mat> rois(contours.size());
        int max_area = 0;
        int max_index = 0;
        for( int i = 0; i< contours.size(); i++)
        {
            Scalar color = Scalar( 0, 255, 0);
            int rec_width = boundRect[i].br().x - boundRect[i].tl().x;
            int rec_height = - boundRect[i].tl().y + boundRect[i].br().y;
            
//            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
//            rectangle( src, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
            
            if (max_area < rec_width*rec_height)
            {
                max_area = rec_width*rec_height;
                max_index = i;
            }
        }
        if (max_area != 0)
        {
            Scalar color = Scalar( 0, 255, 0);
            int rec_width = boundRect[i].br().x - boundRect[i].tl().x;
            int rec_height = - boundRect[i].tl().y + boundRect[i].br().y;

            Rect rec(boundRect[i].tl().x, boundRect[i].tl().y, rec_width, rec_height);
            roi = src(rec);
            resize(roi, roi, cvSize(300, 300));

            imshow("ROI", roi);
            
        }
        
        imshow("View", src);
        imshow("Threshold", imgThresholded);
        if (waitKey(30) == 'a') k--;
        else if (waitKey(30) == 32) k++;
    }
    cv::destroyAllWindows();
}


//        inRange(imgHSV, Scalar(180, 120, 0),
//                Scalar(195, 125, 1),
//                imgThresholded);
