/******************************************************************************

  ArDrone Auto

  Copyright (c) 2007-2014 Shuhui Bu <bushuhui@nwpu.edu.cn>
  http://www.adv-ci.com

  ----------------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/


#include <cmath>
#include <time.h>

#include "ardrone_FlightTask.h"
#include "cv_color_reco.h"
#include "utils.h"

#define PI 3.141592653

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int FlightTask::keyEvent(InputEvent *event)
{
    if( event != NULL ){
        if( event->type == OSA_KEY_PRESS ) {
            //exit program
            if( event->code == OSA_VK_ESCAPE ) {
                m_heli->land();
                m_stopped = true;
            }

            //save images
            if( event->code == OSA_VK_p ){ m_rawimage->saveBmp(); }

            //attituds control
            if( event->code == OSA_VK_a){ yaw = -20000.0;}
            if( event->code == OSA_VK_d){ yaw = 20000.0;}
            if( event->code == OSA_VK_LEFT){roll = -20000.0;}
            if( event->code == OSA_VK_RIGHT){roll = 20000.0;}
            if( event->code == OSA_VK_UP){pitch = -20000.0;}
            if( event->code == OSA_VK_DOWN){pitch = 20000.0;}
            if( event->code == OSA_VK_s){ height = 20000.0;}
            if( event->code == OSA_VK_w){ height = -20000.0;}

            //change camera
            if( event->code ==OSA_VK_z){m_heli->switchCamera(0);}
            if( event->code ==OSA_VK_x){m_heli->switchCamera(1);}
            if( event->code ==OSA_VK_c){m_heli->switchCamera(2);}
            if( event->code ==OSA_VK_v){m_heli->switchCamera(3);}

            //take off & land
            if( event->code ==OSA_VK_ENTER){
                m_takeoff ^= 1;
                if(m_takeoff == true)
                    m_heli->takeoff();
                else
                    m_heli->land();
            }

            //autofly or not
            if( event->code == OSA_VK_SPACE){ m_autoFly ^= 1;}
        }
    }

    return 0;
}

//save flight data and control parameters
void FlightTask::save_flight_data( FILE *fp,const char *filename)
{
    time_t t;
    char str_time[200];
    tm *now;

    fp = fopen(filename,"a+");
    if(fp == NULL){
        printf("can not open %s file\n",filename);
        exit(1);
    }

    //get current time
    time(&t);
    now = localtime(&t);
    strftime(str_time,200,"%Y-%m-%d %H-%M-%S",now);

    fprintf(fp,"%s : \n",str_time);
    fprintf(fp,"x  = %5lf,y  = %5lf,z   = %5lf\n",helidata.x,helidata.y,helidata.z);
    fprintf(fp,"Vx     = %5lf,Vy   = %5lf,Vz  = %5lf\n",helidata.vx,helidata.vy,helidata.vz);
    fprintf(fp,"pitch  = %5lf,roll = %5lf,yaw = %5lf,height = %5lf\n",helidata.theta,helidata.phi,helidata.psi,helidata.altitude);//
    fprintf(fp,"battery= %5lf\n",helidata.battery);
    fprintf(fp,"\n");
}

void FlightTask::save_control_param(FILE *fp,const char *filename)
{
    time_t t;
    char str_time[200];
    tm *now;

    fp = fopen(filename,"a+");
    if(fp == NULL){
        printf("can not open %s file\n",filename);
        exit(1);
    }

    //get current time
    time(&t);
    now = localtime(&t);
    strftime(str_time,200,"%Y-%m-%d %H-%M-%S",now);

    fprintf(fp,"%s : ",str_time);
    fprintf(fp,"pitch = %5f,roll = %5f,yaw = %5f,height = %5f\n",pitch,roll,yaw,height);
    fprintf(fp,"\n");
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
 *track color
 */

//define boundary
#define CEILING_LIMIT 60 //:cm
#define LOWER_LIMIT 30  //:cm


//calculate distance
double cal_distance_fore(int n)
{
    double n0 = 2550.0;
    double init_distance = 30.0;
    double distance = 0;

    distance = init_distance*sqrt(n0/n);

    return distance;
}

//auto control
int FlightTask_ColorTrack::auto_control(SPixelPosition pos,
                                        float &pitch,float &roll,float &yaw,
                                        float &height) {
    double distance = 0;
    if(pos.max_size <= 20){
        number++;
        if(number>=30){
             printf("can not find object!\n");
             //change camera
             m_camera ^= 1;
        }
        return 0;
    }

    //once detect object,set number to zero
    number = 0;

    if(!m_camera){
        yaw    = 100*(pos.x - 320);
        height = 100*(pos.y - 184);
        roll  =  40*(pos.x - 320);

        distance = cal_distance_fore(pos.max_size);

        if(distance > CEILING_LIMIT&&distance < 300)
            pitch = -100*(distance - CEILING_LIMIT);
        if(distance < LOWER_LIMIT)
            pitch = 200*(LOWER_LIMIT - distance);

        dbg_p("dis = %lf,max_size = %d,pitch = %f\n",distance,pos.max_size,pitch,height);

    }
    else{
        if(distance >CEILING_LIMIT)
            height = -1000*(distance - CEILING_LIMIT);
        if(distance < LOWER_LIMIT)
            pitch = 4500*(LOWER_LIMIT - distance);

        yaw = 0;
        roll  = 100*(pos.x - 320);
        pitch = 100*(pos.y - 184);
    }

    last_dis = distance;

    return 0;
}



int FlightTask_ColorTrack::keyEvent(InputEvent *event)
{
    InputEvent _event;

    if(0 == m_win->getEvent(_event)){
        //set pixel to find
        if(_event.type == OSA_MOUSE_DBLCLICK){

            learn_pixel[0] = m_rawimage->data[3*(m_rawimage->width*_event.y_pos+_event.x_pos)+0];
            learn_pixel[1] = m_rawimage->data[3*(m_rawimage->width*_event.y_pos+_event.x_pos)+1];
            learn_pixel[2] = m_rawimage->data[3*(m_rawimage->width*_event.y_pos+_event.x_pos)+2];

            dbg_p("x, y = %d, %d\n", _event.x_pos, _event.y_pos);
            dbg_p("learn_pixel = %d %d %d\n", learn_pixel[0], learn_pixel[1], learn_pixel[2]);
        }

        //reset color map
        if(_event.type == OSA_KEY_PRESS){
            if(_event.code == OSA_VK_r)
                  m_recognition.resetColorMap();
        }

        // call parent class event processing
        FlightTask::keyEvent(&_event);
    }
}

int FlightTask_ColorTrack::videoControl(Mat *img)
{
    SPixelPosition pos;

    if( last_dis > 300||last_dis < 3 ){
        pitch = roll = yaw = height = 0;
        m_heli->setAngles(pitch,roll,yaw,height);
    } else {
        if(  (last_dis < 30 || last_dis > 60) ) {
            pitch *= 0.5;
            roll *= 0.5;
            yaw *= 0.5;
            height *= 0.5;
        } else {
            pitch *= 0.1;
            roll *= 0.1;
            yaw *= 0.1;
            height *= 0.1;
        }

        m_heli->setAngles(pitch,roll,yaw,height);
    }

    m_recognition.learnPixel(learn_pixel);
    pos = m_recognition.findSegment(img);

    //pos = color_reco(m_rawimage, learn_pixel, &m_recognition);

    line(*img,Point(pos.x-15,pos.y),Point(pos.x+15,pos.y),Scalar(0,0,0),2,8,0);
    line(*img,Point(pos.x,pos.y-15),Point(pos.x,pos.y+15),Scalar(0,0,0),2,8,0);

    // auto control
    if( m_autoFly ) {
  //      pitch = roll = yaw = height = 0;
        auto_control(pos,
                     pitch,roll,yaw,height);

        // send commands
        m_heli->switchCamera(m_camera);
        m_heli->setAngles(pitch,roll,yaw,height);
    }

    // process keys
    keyEvent();

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
 * vanish point
 */

FlightTask_VanishPoint::FlightTask_VanishPoint():FlightTask()
{
    vp      = Point(0,0);
    p_left  = Point(0,0);
    p_right = Point(0,0);
}

//detect key event
int FlightTask_VanishPoint::keyEvent(InputEvent *event)
{
    InputEvent _event;

    if(0 == m_win->getEvent(_event)){
        // call parent class event processing
        FlightTask::keyEvent(&_event);
    }

    return 0;
}

int FlightTask_VanishPoint::videoControl(Mat *img){
    Point vp_pre  = vp,p_right_pre = p_right,p_left_pre = p_left;
    Point vp_temp(0,0),p_right_temp(0,0),p_left_temp(0,0);

    int find_vp = 0;

    //send commands
//    pitch = roll = yaw = height = 0;
//    m_heli->setAngles(pitch,roll,yaw,height);

    find_vp = detect_bottom_line(*img,vp_temp,p_left_temp,p_right_temp);

    int center_x = img->cols/2;
    int center_y = img->rows/2;

    if(find_vp == 0){
        int d1 = (center_x - vp_temp.x)*(center_x - vp_temp .x)+(center_y -vp_temp.y)*(center_y - vp_temp.y);
        int d2 = (center_x - vp_pre.x)*(center_x - vp_pre.x)+(center_y -vp_pre.y)*(center_y - vp_pre.y);
       if(d1 > d2+300){
           vp      = vp_pre;
           p_right = p_right_pre;
           p_left  = p_left_pre;
       }
       else{
           vp = vp_temp;
           p_right = p_right_temp;
           p_left  = p_left_temp;
       }

        circle(*img,Point(vp.x,vp.y),4,Scalar(0,255,0),3);
        line(*img,vp,p_right,Scalar(0,0,255),2);
        line(*img,vp,p_left,Scalar(0,0,255),2);

        dbg_p("vp = %d %d\n vp_pre = %d %d\n",vp.x,vp.y,vp_pre.x,vp_pre.y);
    }

    //if we can not detect vanish point,we set the pitch zero
#if 0
    if(vp.x<0||vp.y<0||vp.x>img->cols||vp.y>img->rows){
        pitch = 0;
        roll  = 0;
        height = 0;
        yaw   = 10000;
    }
    if(vp.x - center_x > 20 ) {
        pitch = 0;
        yaw = 0;
        height = 0;
        roll = 100*(vp.x - center_x);
    }
    if(vp.y - center_y>20 ){
        pitch = 0;
        roll  = 0;
        yaw = 0;
        height = 100*(vp.y - center_y);
    }
#endif

    if(m_autoFly){
        if(find_vp == 0){
            //control parameters
            pitch  = -8000;
            roll   = 100*(vp.x - center_x);
            height = 100*(vp.y - center_y);

            //compute the yaw
            double hx = 0,tan_theta = 0,theta = 0;
            double dx = 0,dy = 0;

            dx = p_right.x - p_left.x;
            dy = p_left.y  - p_right.y;

            hx = (vp.x*dx-vp.y*dy+p_left.x*dy*dy/dx+p_left.y*dy)/(dy*dy/dx + dx);

            //tan_theta = std::sqrt((hx - center_x)*(hx - center_x)/((img->rows - vp.y)*(img->rows - vp.y)));
            if(img->rows - vp.y > 0)
                tan_theta   = (hx - center_x)/(img->rows - vp.y);

            theta = std::atan(tan_theta)*180/PI;

            yaw = theta * 100;
        }
        else
        {
            pitch = roll = yaw = height = 0;
        }
    }

    // send commands
    m_heli->setAngles(pitch,roll,yaw,height);

    // process keys
    keyEvent();

    return 0;
}

int FlightTask_VanishPoint::getVanishPoint(Point vanishPoint)
{
    vanishPoint = vp;

    return 0;
}


int FlightTask_VanishPoint::detect_bottom_line(Mat &image,Point &vp,
                                                 Point &p_left,Point &p_right)
{
    Mat img_gray,img_equ_hist,img_gaussi,img_canny;

    vector<Vec4i> lines;
    int tolerance = 30;

    //create gray image
    img_gray.create(image.rows,image.cols,CV_8UC1);

    //apply gaussian blur
    GaussianBlur(image,img_gaussi,Size(5,5),4,4);

    if(img_gaussi.channels()==3){
        cvtColor(img_gaussi,img_gray,CV_BGR2GRAY);
   }
    else
        img_gray = img_gaussi;

    //histogram equalize
    equalizeHist(img_gray,img_equ_hist);

    //apply threshold
 //   threshold(img_equ_hist,img_thresh,30,255,CV_THRESH_BINARY_INV);

    Canny(img_equ_hist,img_canny,40,80);

    //apply hough transform
    HoughLinesP(img_canny,lines,1,CV_PI/180,50,40,10);

    //draw the hough result
    vector<Vec4i>::iterator it = lines.begin();
    int line_num = 0;

    vector<double> data;
    for(;it!=lines.end();it++){
        Vec4i l = *it;
        if(fabs(l[0]-l[2])<tolerance||fabs(l[1]-l[3])<tolerance)
            continue;

        for(int i=0;i<4;i++){
            data.push_back(l[i]);
        }

        //draw the start and end point
//        circle(image,Point(l[0],l[1]),2,Scalar(0,0,255));
//        circle(image,Point(l[2],l[3]),2,Scalar(0,0,255));

        //draw the line
        line(image,Point(l[0],l[1]),Point(l[2],l[3]),Scalar(255,0,0),1,CV_AA);
        line_num++;
    }

    if(line_num == 0) { vp = Point(0,0);return -1;};

    int i = 0,j = 0;
    long N = line_num;
    int K =2,D = 1;

    double** slope = (double**)calloc(N,sizeof(double*));
    for(i =0;i<N;i++){
        slope[i] = (double*)calloc(D,sizeof(double));
    }
    for(int i=0;i<N;i++){
        for(int j=0;j<D;j++){
            slope[i][j] = (data[i*4+3]-data[i*4+1])/(data[i*4+2]-data[i*4]);
        }
    }

    int* label = (int*)calloc(N,sizeof(long int));
    double** center  = (double**)calloc(K,sizeof(double*));

    for(i = 0;i < K;i++){
        center[i]   = (double*)calloc(D,sizeof(double));
    }

    //cluster using k-means algorithm
    k_means(slope,K,N,D,center,label);

    //define the slope
    double k0 = center[0][0],k1 = center[1][0];
    int n0 = 0,n1 = 0;

    if(fabs(atan(k0)-atan(k1)*180/PI)<20){vp = Point(0,0);return -1;}

    double sum_x_0 =0,sum_y_0 = 0,sum_x_1 = 0,sum_y_1 = 0;
    for(i =0;i<N;i++){
        if(label[i] == 0){
            sum_x_0 += data[i*4]+data[i*4+2];
            sum_y_0 += data[i*4+1]+data[i*4+3];
            n0++;
        }
        else{
            sum_x_1 += data[i*4]+data[i*4+2];
            sum_y_1 += data[i*4+1]+data[i*4+3];
            n1++;
        }
    }

    sum_x_0 /= (n0*2);
    sum_x_1 /= (n1*2);
    sum_y_0 /= (n0*2);
    sum_y_1 /= (n1*2);

    double b0 = sum_y_0 - k0*sum_x_0,b1 = sum_y_1 - k1*sum_x_1;

    //cal the left and right points
     p_left.y  = p_right.y = image.rows;
     p_left.x  = (p_left.y-b0)/k0;
     p_right.x = (p_right.y -b1)/k1;

     int temp =  p_right.x;

     p_right.x = p_left.x > p_right.x ? p_left.x:p_right.x;
     p_left.x = p_left.x < temp ? p_left.x:temp;

//     circle(image,p_left,4,Scalar(255,255,0));
//     circle(image,p_right,4,Scalar(255,255,0));

     vp.x = -1*(b0-b1)/(k0-k1);
     vp.y = k0*vp.x + b0;

 //   circle(image,Point(vp.x,vp.y),4,Scalar(0,255,0),3);

    if(vp.x<0||vp.y <0||vp.x>image.cols||vp.y>image.rows){vp = Point(0,0);return -1;}

    //if find vanish point,draw it
//    circle(image,Point(vp.x,vp.y),4,Scalar(0,255,0),3);
//    line(image,vp,p_right,Scalar(255,0,0),2);
//    line(image,vp,p_left,Scalar(255,0,0),2);

    //free space
    for(i =0;i<N;i++){
        free(slope[i]);
    }
    for(i =0;i<K;i++){
        free(center[i]);
    }
    free(label);

    return 0;
  }

int FlightTask_VanishPoint::k_means(double** data,int K,long int N,int D,
            double **now_center,int *label)
{
    double tolerance = 1e-6;

    int* counter   = (int*)calloc(K,sizeof(int)); //the size of every cluster

    double** cluster_data = (double**)calloc(K,sizeof(double*));

    int i = 0;
    int j = 0;
    int k = 0; //define the loop flag vairable
               //i--cluster number,j--data number,k--dimision

    double last_error = 0;
    double now_error  = 0;//define the square sum of distance

    //pick K data points as the initial center
    for(i = 0,j = 0;i < K;j += N/K,i++){

        cluster_data[i] = (double*)calloc(D,sizeof(double));

        for(k = 0;k < D;k++){
            now_center[i][k] = data[j][k];
        }

    }

    do{
        //update the error
        last_error = now_error;
        now_error  = 0;

        //clear the counter and cluster_data
        for(i = 0;i < K;i++){
            counter[i] = 0;
            for(k = 0;k < D;k++){
                cluster_data[i][k] = 0;
            }
        }

        //compute the closest cluster and update the label
        for(j = 0;j < N;j++){
            double min_distance = DBL_MAX;
            for(i = 0;i < K;i++){
                double distance = 0;
                for(k = 0;k < D;k++){
                    distance += pow(data[j][k]-now_center[i][k],2);
                }
                if(distance < min_distance){
                    min_distance = distance;
                    label[j]     = i;
                }
            }

        //update the number of every cluster
         counter[label[j]]++;

        //update the cluster size and cluster data
            for(k = 0;k < D;k++){
                cluster_data[label[j]][k] += data[j][k];
            }

            now_error += min_distance;
        }

        //update the cluster center
        for(i = 0;i < K;i++){
            for(k = 0;k < D;k++){
                now_center[i][k] = cluster_data[i][k]/counter[i];

            }
        }

    }while(fabs(last_error-now_error) > tolerance);

    //free the space
    for(i = 0;i < K;i++){
        free(cluster_data[i]);
    };

    free(counter);
    return 0;

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
 *detect marker
 */

FlightTask_DetectMarker::FlightTask_DetectMarker():FlightTask()
{
    MarkerSize = -1;
    last_dis   = 0;
    CameraParameters();
}

int FlightTask_DetectMarker::setCameraParameters(string filePath)
{
    CamParam.readFromXMLFile(filePath);
    return 0;
}

int FlightTask_DetectMarker::setMarkersize(double _size)
{
    MarkerSize = _size;
    return 0;
}

int FlightTask_DetectMarker::keyEvent(InputEvent *event)
{
    InputEvent _event;
    if(0 == m_win->getEvent(_event)){
        // call parent class event processing
        FlightTask::keyEvent(&_event);
    }

    return 0;
}

int FlightTask_DetectMarker::videoControl(Mat *img)
{
    //send commands
    if(!(last_dis < LOWER_LIMIT||last_dis > CEILING_LIMIT)){
        pitch = roll = yaw = height = 0;
        m_heli->setAngles(pitch,roll,yaw,height);
    }

    if(last_dis > 300){
        pitch = roll = yaw = height = 0;
        m_heli->setAngles(pitch,roll,yaw,height);
    }

    MarkerDetector MDetector;
    vector<Marker> Markers;
    float rotate[3] = {0};
    float translation[3] = {0};

    MDetector.setThresholdParams(6,7);

    //detect markers
    MDetector.detect(*img,Markers,CamParam,MarkerSize);

    Marker maker_;

    for(int n=0;n<Markers.size();n++){
        maker_ = Markers[n];
        for(int i= 0;i<3;i++){
           rotate[i]   = maker_.Rvec.ptr<float>(0)[i];
           translation[i] = maker_.Tvec.ptr<float>(0)[i];
        }
        break;
    }

    //draw markers
    DrawMarkerInfo(*img,Markers,CamParam,MarkerSize,true,true,true);

    keyEvent(NULL);

    //control parameters
    if(m_autoFly){
        /////////////////////////////////////////////////////////////////        
        if(Markers.size()!=0){
            if(translation[2] < LOWER_LIMIT/100.0){
                pitch = 10000*(LOWER_LIMIT/100.0 - translation[2]);
            }

            if(translation[2] > CEILING_LIMIT/100.0&&translation[2] < 300){
                pitch = 10000*(CEILING_LIMIT/100.0 - translation[2]);
            }

            if(translation[2] < CEILING_LIMIT/100.0&&translation[2] > LOWER_LIMIT/100.0){
                pitch = 0;
            }

            yaw   = (maker_.getCenter().x - 320)*30;   //the relation between these angles are complicated and still need  to be modified
            height = (maker_.getCenter().y - 184)*100;

//            yaw    = rotate[2]*100;

            last_dis = translation[2];
        }
        else{
              pitch = roll = yaw = height = 0;
        }

        printf("trans[2] = %lf,rot[2] = %lf\n",translation[2],rotate[2]);
        printf("pitch = %lf,roll = %lf,yaw = %lf,gaz = %lf\n",pitch,roll,yaw,height);
        /////////////////////////////////////////////////////////////////
    }

//    printf("m_autofly = %d\n",m_autoFly);
    m_heli->setAngles(pitch,roll,yaw,height);

    return 0;
}

int FlightTask_DetectMarker::DrawMarkerInfo(Mat &InImage,
                                            std::vector<Marker> &Markers,
                                            CameraParameters CamParam,
                                            double MarkerSize,
                                            bool DrawCenter,
                                            bool DrawBoundary,
                                            bool Draw3dCube)
{
    //Insert following code to get the center of the marker(by myself)
    if(DrawCenter){
        std::vector<Point2f> Marker_center;
        for(unsigned int i=0;i<Markers.size();i++){
            cv::Point2f center;
            center = Markers[i].getCenter();
            Marker_center.push_back(center);
            cv::circle(InImage,center,2,Scalar(0,255,0),2);
//            std::cout << "center:";
//            std::cout << "("<<center.x<<","<<center.y<<")"<<endl;
        }
    }

    //for each marker, draw info and its boundaries in the image
    if(DrawBoundary){
        for (unsigned int i=0; i<Markers.size(); i++) {
//            cout << Markers[i] << endl;
            Markers[i].draw(InImage, Scalar(0,0,255), 2);
        }
    }

   //draw a 3d cube in each marker if there is 3d info
    if(Draw3dCube){
        if ( CamParam.isValid() && MarkerSize!=-1)
            for (unsigned int i=0;i<Markers.size();i++) {
//                CvDrawingUtils::draw3dCube(InImage,Markers[i],CamParam);
                CvDrawingUtils::draw3dAxis(InImage,Markers[i],CamParam);
            }
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////
///////////////////////////video record/////////////////////////////////////

FlightTask_RecordVideo::FlightTask_RecordVideo():FlightTask()
{
    startRecord = false;
    endRecord   = false;
    images.clear();
}

int FlightTask_RecordVideo::keyEvent(InputEvent *event)
{
    InputEvent _event;
    if(0 == m_win->getEvent(_event)){
        if(_event.type == OSA_KEY_PRESS){
            if(_event.code == OSA_VK_P){
                startRecord ^= 1;
                cout << "record" << endl;
            }
            cout << "key pressed" <<endl;
        }

        FlightTask::keyEvent(&_event);
    }

    return 0;
}

int FlightTask_RecordVideo::videoControl(Mat *img)
{
    cout << "start record = "<< startRecord << endl;

    if(startRecord == true){
        Mat frame;
        frame.create(Size(img->cols,img->rows),CV_8UC3);

        imwrite("temp.png",*img);
        frame = imread("temp.png");

        images.push_back(frame);//save images
      }

    if(startRecord == false && images.size() != 0/* && endRecord == false*/)
    {
        cout << "start to generate video ..." << endl;
        if(!videoGenerate())
            cout << "video generated completely" << endl;
        else
            cout << "video generated failed" << endl;

       // endRecord = true;
    }


    keyEvent(NULL);

    return 0;
}

int FlightTask_RecordVideo::videoGenerate()
{
    VideoWriter video;
    vector<Mat>::iterator it = images.begin();
    string videoName;

#if 0
    cout << images.size() <<endl;
    for(int i=0;i<images.size();i++){
        char name[20];
        string str;
        sprintf(name,"%d",i);
        str = string(name) + ".png";
        imwrite(str,images[i]);
    }
#endif

    Mat frame = *it;
    int fps = 10;

    time_t t;
    char str_time[200];
    tm *now;

    //get current time
    time(&t);
    now = localtime(&t);
    strftime(str_time,200,"%Y-%m-%d-%H-%M-%S",now);

    videoName = string(str_time)+".avi";
    cout << videoName << endl;

    video.open(videoName,CV_FOURCC('M','J','P','G'),fps,Size(frame.cols,frame.rows));
    if(!video.isOpened()){
        cout << "can not open video writer" <<endl;
        return -1;
    }

    for(it = images.begin();it != images.end();it++){
        frame = *it;

        video.write(frame);
    }

    images.resize(0);
    video.release();

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////landing/////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

int FlightTask_Landing::videoControl(Mat *img)
{
    MarkerDetector MDetector;
    vector<Marker> Markers;
    Markers.resize(0);

    MDetector.setThresholdParams(6,7);

    //detect markers
    MDetector.detect(*img,Markers,CamParam,MarkerSize,false);

    DrawMarkerInfo(*img,Markers,CamParam,0.05,true,true,true);

    if(m_autoFly){
       calControlParam(Markers);

       m_heli->switchCamera(m_camera);
       m_heli->setAngles(pitch,roll,yaw,height);

       printf("pitch = %f,roll = %f,yaw = %f,height = %f\n",
              pitch,roll,yaw,height);
    }

    keyEvent();
    return 0;
}

int FlightTask_Landing::calControlParam(vector<Marker> &Markers)
{
    if(Markers.size() > 0)
        isMarkerFound  = 1;
    else
        isMarkerFound  = 0;

//    printf("marker size = %d\n",Markers.size());

    if(isMarkerFound){
        nLostMarker = 0; //once detected markers,reset the nLostMarker

        Marker _marker;

        _marker = getMarkerWithLeastId(Markers);//get the marker with least id(don't include markers that has been detected)
        if(_marker.data() == NULL)
            return -1;

        cout << "id = " << _marker.id <<endl;

        calControlParam(_marker); //calculate control parameter if marker detected
    }
    else {
        nLostMarker++;
        if(nLostMarker > 100){
            nLostMarker = 0;

            m_camera    ^= 1;          //change camera if current camera can not detect any marker
        }

        roll = yaw = height = 0.0;
        pitch = 200;
    }


    return 0;
}

int FlightTask_Landing::calControlParam(Marker _marker)
{
    rVec   = _marker.Rvec;
    tVec   = _marker.Tvec;

    const double thres_1 = 40,thres_2 = 2;

    Mat_<double> tVecCm(1,3);
    for(int i=0;i<3;i++){
        tVecCm.at<double>(0,i) = tVec.at<double>(0,i)*100;//convert the scale to centimeter
    }

    cout << "T" << tVecCm <<endl;

    if(0 == m_camera){   //forward camera
        height = 0;
        if(abs(_marker.getCenter().x - 320) > thres_1){
            yaw  = 100*(_marker.getCenter().x - 320);
//            roll  = 10*(_marker.getCenter().x - 320);

            pitch = 200;
        }
        else {
            double dist = norm(Point3_<double>(tVecCm.at<double>(0,0),
                                               tVecCm.at<double>(0,1),
                                               tVecCm.at<double>(0,2)));
            cout << "dist = " << dist << endl;

            double deltaDist = dist - tVecCm.at<double>(0,2);
            if( deltaDist > thres_2 ){
                pitch = 100*deltaDist;

                yaw   = 50*(_marker.getCenter().x - 320);
  //              roll  = 10*(_marker.getCenter().x - 320);
            }
            else {
               m_heli->land();

               detectedId.insert(_marker.id);
               printf("\n-----------------landed----------------\n");
            }
        }
    }
    else {               //downward camera
        height = 0;

        roll = 40*(_marker.getCenter().x - 320);
        yaw  = 20*(_marker.getCenter().x - 320);
        pitch = 80*(_marker.getCenter().y - 184);

        double dist = norm(Point3_<double>(tVecCm.at<double>(0,0),
                                           tVecCm.at<double>(0,1),
                                           tVecCm.at<double>(0,2)));
        cout << "dist = " << dist << endl;

        double deltaDist = dist - tVecCm.at<double>(0,2);
        if(deltaDist < thres_2){
            m_heli->land();

            detectedId.insert(_marker.id);
            printf("\n-----------------landed----------------\n");
        }

    }
    return 0;
}

Marker FlightTask_Landing::getMarkerWithLeastId(vector<Marker> &Markers)//
{
    Marker _marker;
    int id = 1024;

    for(int i=0;i<Markers.size();i++){
        Marker _m = Markers[i];

        if(detectedId.find(_m.id) == detectedId.end()){ //
            if((id > _m.id)&&(_m.id > 0)){
                id      = _m.id;
                _marker = _m;
            }
        }

    }

    if(id == 1024)
        detectedId.clear();

    return _marker;
}

