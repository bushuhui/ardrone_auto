#ifndef COLOR_RECO_H
#define COLOR_RECO_H

#include "ardrone_CRecognition.h"

//assert functions and data format defined by myself
SPixelPosition color_reco(CRawImage* image,unsigned char* learn_pixel, CRecognition *rec);

//user test
int test_image(int argc,char* argv[]);
int test_video(int argc,char* argv[]);

//detect ellipse
RotatedRect ellipse_detect(Mat src_img);


struct user_data{
    int w;                  //the width of image
    unsigned char* data;    //the data region pointer of image
    unsigned char* pixel;   //the pixel you want to trace
};

//callback function of mouse event
void on_mouse(int event,int x,int y,int flags,void* param);//the callback of LMOUSEDOWN event

#endif // COLOR_RECO_H
