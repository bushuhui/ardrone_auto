#include <cv.h>
#include <highgui.h>

#include "cv_color_reco.h"

using namespace cv;

//int main(int argc,char* argv[])
//{
//    if(!test_video(argc,argv))
//        printf("test video successfully!\n");
//    test_image(argc,argv);
//}

//trace color region by given pixel
SPixelPosition color_reco(CRawImage* image,unsigned char* learn_pixel, CRecognition *rec)
{
    SPixelPosition pos;

    rec->learnPixel(learn_pixel);
    pos = rec->findSegment(image);


    //return the center of the objected region
    return pos;
}

//extract a single channel of an image
int extract_channel(Mat image_src,Mat &image_dst,int channel)
{
   int w,h;

   w = image_src.cols;
   h = image_src.rows;

   if(image_src.channels()!=3){
       printf("can not extract the channel\n");
       return -1;
   }
   for(int i =0;i<w*h;i++){
       image_dst.data[i] = image_src.data[i*3+channel];
   }
   return 0;
}

//draw the contour of objects in a picture
RotatedRect ellipse_detect(Mat src_img)
{
//    if(src_img.empty()) return -1;

    Mat gray_img, bin_img;
    RotatedRect box ;
    cvtColor(src_img, gray_img, CV_BGR2GRAY);

    std::vector<std::vector<cv::Point> > contours;

    threshold(gray_img, bin_img, 0, 255, cv::THRESH_BINARY|cv::THRESH_OTSU);

    findContours(bin_img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    for(int i = 0; i < contours.size(); ++i) {
      size_t count = contours[i].size();
      if(count < 150 || count > 1000) continue;

      Mat pointsf;
      Mat(contours[i]).convertTo(pointsf, CV_32F);
      box = fitEllipse(pointsf);
    }
    return box;
}

int test_video(int argc,char* argv[])
{
    VideoCapture video;
    Mat image_in,*image_out;
    user_data param;

//    RotatedRect box;

    unsigned char learn_pixel[3]={255,0,0};
    SPixelPosition pos;

    int w = 0,h = 0;
    int time_delay = 20;
    char key = 0;

    if(argc < 2){
        printf("please input the video name\n");
        exit(1);
    }
    video.open(argv[1]);
    if(!video.isOpened()){
        printf("Failed to open video file: %s\n",argv[1]);
        exit(1);
    }

    //get first frame
    video >> image_in;

    w = image_in.cols;
    h = image_in.rows;

//    box = ellipse_detect(image_in);
//    ellipse(image_in, box, Scalar(0,0,0), 2, CV_AA);

    CRawImage image(w,h);
    image.from_mat(&image_in);

    CRecognition rec_color;
    pos = color_reco(&image,learn_pixel, &rec_color);
    printf("pos.x = %d,pos.y = %d\n",pos.x,pos.y);

    image.to_mat(*image_out);

    imshow("color_detection",*image_out);
    key = waitKey(time_delay);

    while(key != 27 ){

        param.w = image_in.cols;
        param.data = image_in.data;
        param.pixel = learn_pixel;

        setMouseCallback("color_detection",on_mouse,&param);
        //get next frame
        video >> image_in;
        if(image_in.empty())break;

        CRawImage image(w,h);
        image.from_mat(&image_in);

        CRecognition rec_color;
        pos = color_reco(&image,learn_pixel, &rec_color);
        printf("pos.x = %d,pos.y = %d\n",pos.x,pos.y);

        image.to_mat(*image_out);

        imshow("color_detection",*image_out);
        key =  waitKey(time_delay);
    }
    return 0;
}
//the callback of LEFTMOUSEDOWN event
void on_mouse(int event,int x,int y,int flags,void* param)
{
    user_data* dat = (user_data*)param;
    int w = dat->w;
    if(event == EVENT_LBUTTONDOWN){
        for(int i =0;i<3;i++){
            dat->pixel[i] = dat->data[(y*w+x)*3+2-i];
        }
    }
}

int test_image(int argc,char* argv[])
{
    Mat img_in,red_channel;
    img_in = imread("test.jpg");

    int w = img_in.cols;
    int h = img_in.rows;
    printf("w = %d,h = %d\n",w,h);

    //extract red channel
    red_channel.create(h,w,CV_8UC1);
    extract_channel(img_in,red_channel,2);

    imshow("original",red_channel);
    imshow("original picture",img_in);
    waitKey();

    CRawImage image(w,h);
    image.from_mat(&img_in);

    unsigned char learn_pixel[3]={255,0,0};
    CRecognition rec_color;
    SPixelPosition pos = color_reco(&image,learn_pixel, &rec_color);
    printf("pos.x = %d,pos.y = %d\n",pos.x,pos.y);

    Mat* img_out;
    image.to_mat(*img_out);

    imshow("color_detection",*img_out);
    waitKey();

    return 0;
}
