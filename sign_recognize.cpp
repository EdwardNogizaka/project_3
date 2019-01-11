

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "library.hpp"
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>



//#include "detectlane.h"

//bool STREAM = true;

//DetectLane *detect;

int skipFrame = 1;
using namespace cv;
bool first = false;
int i = 0;
int minHessian = 4;
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
using namespace cv::xfeatures2d;




int main(int argc, char **argv)
{
    Mat sign_left = imread("turnleft.jpg");
    Mat sign_right = imread("turnright.jpg");

    
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
    int k=1970;
    
//    int k=2330;
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
            
            cvtColor(roi, roi, COLOR_BGR2GRAY);
            threshold( roi, roi, 250, 255, THRESH_BINARY);

            
            
            
            
//            imwrite("sign.png", roi);
            Ptr<SURF> detector = SURF::create( minHessian );
            Mat descriptors_1, descriptors_2;

            vector<KeyPoint> keypoints_1, keypoints_2;
//            detector->detect( roi, keypoints_1 );
            
            detector->detectAndCompute( roi, Mat(), keypoints_1, descriptors_1 );
            detector->detectAndCompute( sign_right, Mat(), keypoints_2, descriptors_2);
            
            FlannBasedMatcher matcher;
            std::vector< DMatch > matches;
            matcher.match( descriptors_1, descriptors_2, matches );

            double max_dist = 0; double min_dist = 100;
            //-- Quick calculation of max and min distances between keypoints
            for( int i = 0; i < descriptors_1.rows; i++ )
            {
                double dist = matches[i].distance;
                if( dist < min_dist ) min_dist = dist;
                if( dist > max_dist ) max_dist = dist;
            }

            std::vector< DMatch > good_matches;
            
            for( int i = 0; i < descriptors_1.rows; i++ )
            {
                if( matches[i].distance <= max(2*min_dist, 0.02) )
                {
                    good_matches.push_back( matches[i]);
                }
            }
            
            cout<<(good_matches.size())<<endl;
            
            
            drawKeypoints( roi, keypoints_1, roi, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
            imshow("ROI", roi);
            imshow("Sign", sign_right);

        }
        
        imshow("View", src);
        imshow("Threshold", imgThresholded);
        if (waitKey(30) == 'a') k--;
        else if (waitKey(30) == 32) k++;
    }
    cv::destroyAllWindows();
}
