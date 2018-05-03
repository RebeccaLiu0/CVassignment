//
//  Barcode2D.cpp
//  learnOpenCv
//
//  Created by 刘楠 on 2018/5/3.
//  Copyright © 2018年 Rebecca. All rights reserved.
//

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <locale>

using namespace std;
using namespace cv;

//functions
Mat imScale(Mat image);
Mat rotate(Mat image, double angle);
vector<Vec3f> circenter(Mat image);
void decode(Mat image);
Mat rotateFinal(Mat image, vector<Vec3f> c);

#define MpixelR(image,x,y) ( (uchar *) ( ((image).data) + (y)*((image).step) ) )[(x)*((image).channels())]
#define MpixelG(image,x,y) ( (uchar *) ( ((image).data) + (y)*((image).step) ) )[(x)*((image).channels())+1]
#define MpixelB(image,x,y) ( (uchar *) ( ((image).data) + (y)*((image).step) ) )[(x)*((image).channels())+2]
#define PI 3.14159265

char encodingarray[64]={' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','x','y','w','z',
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','X','Y','W','Z',
    '0','1','2','3','4','5','6','7','8','9','.'};


//main function
int main( int argc, char* argv[] ) {
    Mat image;
    if (argc ==2) {
        image = imread( argv[1],1);
    }
    else exit(0);
    
//find circle centers on original image
    vector<Vec3f> c = circenter(image);
    for (size_t i = 0; i < c.size(); i++)
    {
        Point center (c[i][0], c[i][1]);
        int radius = c[i][2];
        circle(image, center, radius, Scalar(155, 50, 255), 3, 8, 0);
    }
    namedWindow("show", 1);
    imshow("show", image);
    waitKey(0);
    
//if there is no 3 circles,error
//this code depends on 3 circles
    if(c.size()!=3){
        cout<<"error: there is no three circles."<<endl;
    }
    
    else{
        Mat image1= rotateFinal(image, c);
            image1=imScale(image1);
            decode(image1);
    
    }
}


//get circle center
vector<Vec3f> circenter(Mat image){
    Mat image2;
    cvtColor(image, image2, CV_BGR2GRAY);
    vector<Vec3f> c;
    HoughCircles(image2, c, CV_HOUGH_GRADIENT, 1, image2.rows/16, 100, 30, 1, 30);
    return c;
}

//basic rotate
Mat rotate(Mat image, double angle){
    //int len = max(image.cols, image.rows);
    Mat image1;
    Mat rotmatrix=getRotationMatrix2D(Point((image.cols/2),(image.rows/2)), angle, 1);
    warpAffine(image, image1, rotmatrix, image.size());
   // cout<<angle<<endl;
    return image1;
}

//Scale
Mat imScale(Mat image)
{   Mat imageS;
    int imsize;
    imsize=image.rows;
    if (imsize==1000) return image;
    if (imsize>1000) {resize(image,imageS,Size(1000,1000),0,0,INTER_AREA);}
    else {resize(image,imageS,Size(1000,1000),0,0,INTER_LINEAR);}
    imshow("Final Scaled image",imageS);
    
    return imageS;
}


void decode(Mat image) {
    Mat image2;
    
    //step1: define a 47*47,3chanel,matrix--image2
    //because there are 47*47 useful blocks in the original image
    image2.create(47,47,CV_8UC3);
    int y=0;
    for(int i=30;i<image.rows-30;i+=20){
        int x=0;
        for(int j=30;j<image.cols-30;j+=20){
            int r=0,g=0,b=0;
            for(int m=0;m<20;m++){
                for(int n=0;n<20;n++){
                    r += MpixelR(image,i+m,j+n);
                    g += MpixelG(image,i+m,j+n);
                    b += MpixelB(image,i+m,j+n);
                }
            }
            
            //for each 20*20 block, get the mean r,g,b value
            //if the mean >=128, change the block value to 1 & save it to image2
            if(r/400 >= 128){MpixelR(image2, x, y)=1;}
            if(r/400 < 128){MpixelR(image2, x, y)=0;}
            if(g/400 >= 128){MpixelG(image2, x, y)=1;}
            if(g/400 < 128){MpixelG(image2, x, y)=0;}
            if(b/400 >= 128){MpixelB(image2, x, y)=1;}
            if(b/400 < 128){MpixelB(image2, x, y)=0;}
            x++;
        }
        y++;
    }
    
    
    //step2: decoding image2 to an array pair by pair
    //get rid of the three circles by tagging them 5
    for(int j=0;j<6;j++){
        for(int i=0;i<6;i++){
            MpixelB(image2,i,j)=5;
            MpixelG(image2,i,j)=5;
            MpixelR(image2,i,j)=5;
        }
        for(int i=41;i<47;i++){
            MpixelB(image2,i,j)=5;
            MpixelG(image2,i,j)=5;
            MpixelR(image2,i,j)=5;
        }
    }
    for(int j=41;j<47;j++){
        for(int i=41;i<47;i++){
            MpixelB(image2,i,j)=5;
            MpixelG(image2,i,j)=5;
            MpixelR(image2,i,j)=5;
        }
    }
    
    //get an array of 3-bit strings, including all the useful pixels
    string barDecode[2101];
    int i=0;
    for(int x=0;x<47;x++){
        for(int y=0;y<47;y++){
            stringstream ss;
            if(MpixelB(image2, x, y)!=5){
                ss<<to_string(MpixelB(image2, x, y))<<to_string(MpixelG(image2, x, y))<<to_string(MpixelR(image2, x, y));
                barDecode[i]=ss.str();
                i++;
            }
        }
    }
    
    
    
    //get pairs of strings from last array --barDecode[2101]
    //combine them into a new array of 6-bit strings --barDecode2
    
    string barDecode2[1050];
    long c=0;
    for(long d=0;d<2100;d+=2){
        stringstream ss;
        ss<<barDecode[d]<<barDecode[d+1];
        barDecode2[c]=ss.str();
        c++;
    }
    
    
    
    //step 3: final decoding
    
    //chage string to number & print the letters
    cout<<"decoding result:"<<endl;
    for(int i=0;i<1051;i++){
        int e=stoi(barDecode2[i],0,2); //chage string to decimal number
        cout<<encodingarray[e];
    }
    cout<<endl;
    
}



//final rotate

Mat rotateFinal(Mat image, vector<Vec3f> c) {
    
    vector<float> line(3);
    vector<float> angle(3);
    //get lengths between each two centers
    //get angles between each line and horizen
    line[0]=sqrt((c[0][0]-c[1][0])*(c[0][0]-c[1][0])+(c[0][1]-c[1][1])*(c[0][1]-c[1][1]));
    //angle[0]=(atan(sqrt(pow(c[0][1]-c[1][1],2))/sqrt(pow(c[0][0]-c[1][0],2))))*180/PI;
    angle[0]=atan((c[0][1]-c[1][1])/(c[0][0]-c[1][0]))*180/PI;
    line[1]=sqrt((c[2][0]-c[1][0])*(c[2][0]-c[1][0])+(c[2][1]-c[1][1])*(c[2][1]-c[1][1]));
    //angle[1]=atan(sqrt(pow(c[2][1]-c[1][1],2))/sqrt(pow(c[2][0]-c[1][0],2)))*180/PI;
    angle[1]=atan((c[2][1]-c[1][1])/(c[2][0]-c[1][0]))*180/PI;
    line[2]=sqrt((c[2][0]-c[0][0])*(c[2][0]-c[0][0])+(c[2][1]-c[0][1])*(c[2][1]-c[0][1]));
    //angle[2]=(atan(sqrt(pow(c[2][1]-c[0][1],2))/sqrt(pow(c[2][0]-c[0][0],2))))*180/PI;
    angle[2]=atan((c[2][1]-c[0][1])/(c[2][0]-c[0][0]))*180/PI;
    
    //get the longest line & corresponding angle
    //rotate to an updown image
    long dis=distance(line.begin(),max_element(line.begin(),line.end()));
    
    if (sqrt(pow(angle[dis]-45,2))<2){
        cout<<"do not need to rotate"<<endl;
        return image;
    }
    else{
    Mat image_ro1 = rotate(image,angle[dis]+45);
        //cout<<angle[dis]+45<<endl;
  //  image_ro1 = rotate(image_ro1,45);
    namedWindow("rotate1", 1);
    imshow("rotate1", image_ro1);
    waitKey(0);
    
    
    //then rotate to the right derection
    //no circle on right top
    Mat image1=image_ro1.clone();
    vector<Vec3f> cr = circenter(image1);
    int flag=1;
    while(flag){
        for(int i=0;i<3;i++){
            if (cr[i][0]>image1.rows/2){
                if(cr[i][1]<image1.cols/2){
                    flag=1;
                    //cout<<"there is a circle on top right: flag="<<flag<<endl;
                    //cout<<c[i]<<endl;
                    image1=rotate(image1, 90);
                    break;
                }
                else {flag=0;}
            }
        }
        cr = circenter(image1);
        
    }
    
    namedWindow("rotate2", 1);
    imshow("rotate2", image1);
    waitKey(0);
    
    
    //cut the image
    int x1,y1;
    int xx1,yy1,xx2,yy2;
    long dis_s=distance(line.begin(),min_element(line.begin(),line.end()));
    int len=(int)line[dis_s]/2;
    x1=image1.rows/2;
    y1=image1.cols/2;
    float blank=len*30/470;
    float r = len*40/470;
    xx1 = x1-len-2*blank-r;
    yy1 = y1-len-2*blank-r;
    xx2 = len+x1+2*blank+r;
    yy2 = len+y1+2*blank+r;
    
    Mat finalim = image1(Rect(xx1, yy1,xx2-xx1 , yy2-yy1));
    
    namedWindow("final", 1);
    imshow("final", finalim);
    waitKey(0);
    
    return finalim;
    }
    
}

