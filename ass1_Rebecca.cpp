
//
//  main.cpp
//  learnOpenCv
//
//  Created by 刘楠 on 2018/3/15.
//  Copyright © 2018年 Rebecca. All rights reserved.
//


#include <vector>
#include <set>
#include <algorithm>

#include <stdio.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;
using namespace chrono;


//Macro for Mat structures
#define Pixel(image,x,y) ( (uchar *) ( ((image).data) + (y)*((image).step) ) ) [(x)]

int median(vector<int> set);
Mat medianFilter(Mat image);

int otsuTh(const Mat image);
Mat otsuIm(Mat image);

int objCount(Mat image);



Mat frame;
int main( int argc, char* argv[] ) {
    if(argc == 1){
        //copy camera coding, dealing with Mat frame
        VideoCapture cap;
        cap.open(0);
        if (!cap.isOpened())
        {
            cout << "Failed to open camera" << endl;
            return 0;
        }
        cout << "Opened camera" << endl;
        namedWindow("WebCam", 1);
        cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
        //   cap.set(CV_CAP_PROP_FRAME_WIDTH, 960);
        //   cap.set(CV_CAP_PROP_FRAME_WIDTH, 1600);
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
        //   cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
        //   cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1200);
        cap >> frame;
        printf("frame size %d %d \n",frame.rows, frame.cols);
        int key=0;
        
        double fps=0.0;
        while (1){
            system_clock::time_point start = system_clock::now();
            //for(int a=0;a<10;a++){
            cap >> frame;
            if( frame.empty() )
                break;
            
            char printit[100];
            sprintf(printit,"%2.1f",fps);
            putText(frame, printit, cvPoint(10,30), FONT_HERSHEY_PLAIN, 2, cvScalar(255,255,255), 2, 8);
            
            
            //my coding starts
            Mat image2,image3;
            image2.create(frame.size(),CV_8UC1);
            image3.create(frame.size(),CV_8UC1);
            cvtColor(frame,image2,CV_BGR2GRAY);
            image2 = medianFilter(image2);
            image2 = medianFilter(image2);
            threshold(image2,image3,0,255,CV_THRESH_BINARY | CV_THRESH_OTSU);
            int num;
            num = objCount(image3);
            
            
            stringstream ss;
            ss << num;
            string counum = ss.str();
            putText(image3,counum,cvPoint(10,50),FONT_HERSHEY_PLAIN,4,Scalar(100,255,255),2,8,0);
            
            imshow("WebCam", image3);
            //my coding ends
            
            key=waitKey(1);
            if(key==113 || key==27) return 0;//either esc or 'q'
            
            //}
            system_clock::time_point end = system_clock::now();
            double seconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            //fps = 1000000*10.0/seconds;
            fps = 1000000/seconds;
            cout << "frames " << fps << " seconds " << seconds << endl;
        }
    }
  
  
    
    Mat image;
    
    if (argc ==2) {
        image = imread( argv[1],1);
    }
    
  
    
   // Mat image=imread("Imagestotest/bolts_nuts_1.jpg",CV_LOAD_IMAGE_GRAYSCALE);
    Mat image2,image3;
    image2.create(image.size(),CV_8UC1);
    image3.create(image.size(),CV_8UC1);
    cvtColor(image,image2,CV_BGR2GRAY);
    // cvtColor(image,image2,CVBGR2GRAY);
    
    image2 = medianFilter(image2);
    //image2 = medianFilter(image2);
    image3 = image2.clone();
    image3 = otsuIm(image3);
    //threshold(image2,image3,0,255,CV_THRESH_BINARY | CV_THRESH_OTSU);
    
    
    int num;
    num = objCount(image3);
    
    
    
    /*
     char counum[100];
     sprintf(counum,"%d",num);
     cout<<"sprintf:"<<counum<<endl;
     putText(image3,counum,cvPoint(10,10),FONT_HERSHEY_PLAIN,4,Scalar(0,255,255),2,8,0);
     */
    stringstream ss;
    ss << num;
    string counum = ss.str();
    putText(image3,counum,cvPoint(10,50),FONT_HERSHEY_PLAIN,4,Scalar(100,255,255),2,8,0);
    putText(image2,counum,cvPoint(10,50),FONT_HERSHEY_PLAIN,4,Scalar(100,255,255),2,8,0);
    putText(image,counum,cvPoint(10,50),FONT_HERSHEY_PLAIN,4,Scalar(100,255,255),2,8,0);
    
    
    namedWindow("original", 1);
    imshow("original",image);
    namedWindow("step1_medianFilter", 1);
    imshow("step1_medianFilter",image2);
    namedWindow("step2_otsuThreshold", 1);
    imshow("step2_otsuThreshold",image3);
    
    
    
    waitKey(0);
    return 0;
    
    
}


int median(vector<int> set){
    sort(set.begin(),set.end());
    return (set.size()%2==0 ? (set[set.size()/2]+set[set.size()/2-1])/2 : set[set.size()/2]);
}


Mat medianFilter(Mat image){
    vector<int> set(9);
    uchar z = 0;
    Mat image2;
    image2.create(image.size(),CV_8UC1);
    for(int y=0; y<image.rows; y++){
        for(int x=0; x<image.cols; x++){
            
            if ((x>=1)&&(y>=1)&&(x<(image.cols-1))&&(y<(image.rows-1))){
                set[0] = Pixel(image,x-1,y-1);
                set[1] = Pixel(image,x,y-1);
                set[2] = Pixel(image,x+1,y-1);
                set[3] = Pixel(image,x-1,y);
                set[4] = Pixel(image,x,y);
                set[5] = Pixel(image,x+1,y);
                set[6] = Pixel(image,x-1,y+1);
                set[7] = Pixel(image,x,y+1);
                set[8] = Pixel(image,x+1,y+1);
                
                z=median(set);
                
                // Pixel(image2,x,y-1)=z;
                Pixel(image2,x,y)=z;
                // Pixel(image2,x,y+1)=z;
                
            }
            else Pixel(image2,x,y)=0;
        }
    }
    return image2;
}


//find the otsu's threshold
int otsuTh(const Mat image){
    int pixel=0;
    int T=128;
    // vector<double> b(255);
    double varValue=0;
    double a1=0,a2=0,u1=0,u2=0;
    double hs[256]={0};
    double N=image.rows * image.cols;
    
    //count pixels for each graylevel
    for(int a=0; a< image.rows ;a++){
        for(int b=0; b<image.cols; b++){
            pixel = Pixel(image,a,b);
            hs[pixel]++;
            //  cout<<"pixel:"<<pixel<<endl;
        }
    }
    
    
    //get the variances for each possible threshold t
    for(int t=0; t<=255; t++){
        a1=0;
        a2=0;
        u1=0;
        u2=0;
        
        //background
        for(int i=0;i<=t;i++){
            a1= a1+hs[i];
            u1= u1+i*hs[i];
        }
        
        if(a1==0)
        {
            break;
        }
    
        
        u1=u1/a1;
        a1=a1/N;
        
        //front
        for(int j=t+1; j<255; j++){
            a2 = a2+hs[j];
            u2 = u2+j*hs[j];
        }
        
        if(a2==0) //前景部分像素点数为0时退出
        {
            break;
        }
        
        u2=u2/a2;
        a2=a2/N;
        
        double varValue2=a1*a2*(u1-u2)*(u1-u2);
        
        //  b[t]=a1*a2*(u1-u2)*(u1-u2);
        //  cout<<b[t]<<endl;
        if(varValue<varValue2){
            varValue=varValue2;
            T=t;
            //   cout<<"t:"<<t<<" "<<varValue2<<endl;
        }
        //cout<<"t:"<<t<<" "<<"a1:"<<a1<<" "<<"u1:"<<u1<<"a2:"<<a2<<" "<<"u2:"<<u2<<endl;
        
        
    }
    
    //T= distance(b.begin(),max_element(b.begin(),b.end()));
   // cout<<T<<endl;

    if(T<20) T=70;

    return T;
    
}



//devide one picture into four
Mat otsuIm(Mat image){
    //int t = otsuTh(image);
    int x1=image.cols/2;
    int y1=image.rows/2;
    int x2=image.cols-x1;
    int y2=image.rows-y1;
    Mat image1,image2,image3,image4;
    image1.create(y1, x1, CV_8UC1);
    image2.create(y1, x2, CV_8UC1);
    image3.create(y2, x1, CV_8UC1);
    image4.create(y2, x2, CV_8UC1);


    
    for(int y=0;y<image1.rows; y++){
        for(int x=0; x<image1.cols; x++){
            image1.at<uchar>(y,x)=image.at<uchar>(y,x);
        }
    }
    
    for(int y=0;y<y1; y++){
        for(int x=0; x<x2; x++){
            image2.at<uchar>(y,x)=image.at<uchar>(y,x+x1);
            
        }
    }
    
    for(int y=0;y<image3.rows; y++){
        for(int x=0; x<image3.cols; x++){
            image3.at<uchar>(y,x)=image.at<uchar>(y+y1,x);
        }
    }
    
    for(int y=0;y<image4.rows; y++){
        for(int x=0; x<image4.cols; x++){
            image4.at<uchar>(y,x)=image.at<uchar>(y+y1,x+x1);
        }
    }
    
    
    int t1=otsuTh(image1);
    int t2=otsuTh(image2);
    int t3=otsuTh(image3);
    int t4=otsuTh(image4);
    
    for(int y=0; y<image1.rows;y++){
        for (int x=0; x<image1.cols;x++){
            if(Pixel(image1,x,y)>=t1){
                Pixel(image1, x, y)=255;
            }
            else Pixel(image1, x, y)=0;
        }
    }
    
    for(int y=0; y<image2.rows;y++){
        for (int x=0; x<image2.cols;x++){
            if(Pixel(image2,x,y)>=t2){
                Pixel(image2, x, y)=255;
            }
            else Pixel(image2, x, y)=0;
        }
    }
    
    for(int y=0; y<image3.rows;y++){
        for (int x=0; x<image3.cols;x++){
            if(Pixel(image3,x,y)>=t3){
                Pixel(image3, x, y)=255;
            }
            else Pixel(image3, x, y)=0;
        }
    }
    for(int y=0; y<image4.rows;y++){
        for (int x=0; x<image4.cols;x++){
            if(Pixel(image4,x,y)>=t4){
                Pixel(image4, x, y)=255;
            }
            else Pixel(image4, x, y)=0;
        }
    }
    
    
    for(int y=0;y<image1.rows; y++){
        for(int x=0; x<image1.cols; x++){
            image.at<uchar>(y,x)=image1.at<uchar>(y,x);
        }
    }
    
    for(int y=0;y<image2.rows; y++){
        for(int x=0; x<image2.cols; x++){
            image.at<uchar>(y,x+x1)= image2.at<uchar>(y,x);
        }
    }
    
    for(int y=0;y<image3.rows; y++){
        for(int x=0; x<image3.cols; x++){
            image.at<uchar>(y+y1,x)=image3.at<uchar>(y,x);
        }
    }
    
    for(int y=0;y<image4.rows; y++){
        for(int x=0; x<image4.cols; x++){
            image.at<uchar>(y+y1,x+x1)=image4.at<uchar>(y,x);
        }
    }
    
    cout<<"T1:"<<t1<<" T2:"<<t2<<" T3:"<<t3<<" T4:"<<t4<<endl;
    return image;
}




int objCount(Mat image){
    int counter = -1, s1, s2;
    vector<set<pair<int,int> > > SET;
    Mat A(image.size(),CV_16SC1,-1);
    set<pair<int,int> > tmp;
    
    for(int y = 1; y<image.rows; y++){
        for(int x = 1; x < image.cols; x++){
            if(Pixel(image, x, y) != 0){
                if((Pixel(image, x-1, y) != 0) || Pixel(image, x, y-1) != 0){
                    // s1 = Pixel(A,x-1,y);
                    s1 = A.at<short>(y,x-1);
                    //s1 = A.at<short>(y-1,x);
                    
                    //s2 = Pixel(A,x,y-1);
                    s2 = A.at<short>(y-1,x);
                    //s2 = A.at<short>(y,x-1);
                    
                    //cout<<"s1: "<<s1<<"  s2: "<<s2<<endl;
                    
                    if(s1 != -1){
                        SET[s1].insert(make_pair(x,y));
                        A.at<short>(y,x)=s1;
                        // cout<<"s1: "<<s1<<endl;
                        
                        
                    }
                    if(s2 != -1){
                        SET[s2].insert(make_pair(x,y));
                        // Pixel(A,x,y)=s2;
                        A.at<short>(y,x)=s2;
                        //cout<<"s2: "<<s2<<endl;
                        
                    }
                    if((s1 != s2) && (s1 != -1) && (s2 != -1)){
                        //set_union(SET[s1].begin(), SET[s1].end(), SET[s2].begin(), SET[s2].end(),inserter(tmp, tmp.begin()));
                        set_union(SET[s1].begin(), SET[s1].end(), SET[s2].begin(), SET[s2].end(),inserter(SET[s1], SET[s1].begin()));
                       // SET[s1]=tmp;
                        for(set<pair<int,int> >::iterator iter = SET[s2].begin();  iter != SET[s2].end(); iter++){
                            // Pixel(A,iter->first,iter->second)=s1;
                            A.at<short>(iter->second,iter->first) = s1;
                            
                        }
                        SET[s2].clear() ;
                        //tmp.clear();
                        // cout<<"s2: "<<s2<<endl;
                        //  cout<<"s1: "<<s1<<endl;
                    }
                }
                
                
                else {
                    counter++;
                    tmp.insert((make_pair(x,y)));
                    SET.push_back(tmp);
                    //Pixel(A, x, y) = counter;
                    A.at<short>(y,x) = counter;
                    tmp.clear();
                    // cout<<"counter:"<<counter<<endl;
                }
                
                /*
                else{
                    counter++;
                    SET[counter].insert(make_pair(x,y));
                    A.at<short>(y,x) = counter;
                    
                }
                 */
                
                
                
            }
            
        }
    }
    
    
    
    //Mat image_test(image.size(),CV_8UC3);
    
    
    // vector<set<pair<int,int> > >::iterator it;
    
    int objNum=0;
    
    
    
    //cout<< SET.size()<<endl;
    for(int i=0; i < SET.size(); i++){
        //if(!(SET[i].empty())){
        if(!(SET[i].empty()) && SET[i].size()>50){
            
            
            objNum++;
            // cout<<"\n"<<"SET["<<i<<"]:";
            // cout<<"\n"<<"Objects num is:"<<objNum<<endl;
            
        }
    }
    
    //cout<<"Objects num is:"<<objNum<<endl;
    return objNum;
    
    
    
    //return 0;
    
}

