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

#ifndef __FLIGHTTASK_H__
#define __FLIGHTTASK_H__

#include <stdlib.h>

#include <cv.h>
#include <highgui.h>

#include "ardrone_CHeli.h"
#include "ardrone_CRecognition.h"

#include "GUI_InputEvent.h"
#include "GUI_MainWindow.h"

#include "marker/aruco.h"
#include "marker/aruco_cvdrawingutils.h"

using namespace aruco;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class FlightTask
{
public:
    FlightTask() {
        pitch  = 0;
        roll   = 0;
        yaw    = 0;
        height = 0;

        m_stopped = false;
        m_autoFly = false;
        m_takeoff = false;

        m_camera  = 0;
        m_imgSaved = 0;
    }

    virtual ~FlightTask() {}

    virtual int keyEvent(InputEvent *event = NULL);
    virtual int videoControl(Mat *img) = 0;

    void set_heli(CHeli *heli) { m_heli = heli; }
    void set_gui(MainWindow *win) {m_win = win; }
    void set_rawimage(CRawImage *img) { m_rawimage = img; }

    void save_flight_data( FILE *fp,const char *filename);
    void save_control_param(FILE *fp,const char *filename);

    bool is_stopped(void) { return m_stopped; }

protected:
    CHeli       *m_heli;
    MainWindow  *m_win;

    int        m_camera;
    CRawImage *m_rawimage;
    int        m_imgSaved;

    bool    m_takeoff;//1:takeoff
    bool    m_autoFly;//1:auto fly
    float   pitch,roll,yaw,height;
    bool    m_stopped;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
 *track color
 */

class FlightTask_ColorTrack : public FlightTask
{
public:
    FlightTask_ColorTrack():FlightTask() {

        number = 0;

        learn_pixel[0]=255;
        learn_pixel[1]=0;
        learn_pixel[2]=0;
    }

    ~FlightTask_ColorTrack() { m_heli = NULL; m_win = NULL; }


    int keyEvent(InputEvent *event = NULL);
    int videoControl(Mat *img);

private:
    unsigned char   learn_pixel[3];
    int             number;
    CRecognition    m_recognition;

    double          last_dis;

    int auto_control(SPixelPosition pos,
                     float &pitch, float &roll, float &yaw,
                     float &height);

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
 * vanish point
 */

class FlightTask_VanishPoint : public FlightTask
{
public:
    FlightTask_VanishPoint();

    ~FlightTask_VanishPoint(){
        m_heli = NULL;
        m_win  = NULL;
        m_rawimage = NULL;
    }

    int keyEvent(InputEvent *event = NULL);
    int videoControl(Mat *img);
    int getVanishPoint(Point vanishPoint);

private:
    Point vp;
    Point p_left;
    Point p_right;

    int detect_bottom_line(Mat &image,Point &vp,
                            Point &p_left,Point &p_right);

    int k_means(double** data,int K,long int N,int D,
                double **now_center,int *label);

};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
 *detect marker
 */
class FlightTask_DetectMarker : public FlightTask
{
public:
    FlightTask_DetectMarker();

    ~FlightTask_DetectMarker(){m_heli = NULL;m_win = NULL;m_rawimage = NULL;}

    int keyEvent(InputEvent *event = NULL);
    int videoControl(Mat *img);

    int setMarkersize(double _size);
    int setCameraParameters(string filePath);

protected:
    aruco::CameraParameters CamParam;

    double MarkerSize;
    double last_dis;

    int DrawMarkerInfo(cv::Mat &InImage,
                       std::vector<aruco::Marker> &Markers,
                       aruco::CameraParameters CamParam,
                       double MarkerSize,
                       bool DrawCenter,
                       bool DrawBoundary,
                       bool Draw3dCube);
};

////////////////////////////////////////////////////////////////////////////////
///////////////////////////record video/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class FlightTask_RecordVideo : public FlightTask
{
public :
    FlightTask_RecordVideo();

    ~FlightTask_RecordVideo(){m_heli = NULL;m_win = NULL;m_rawimage = NULL;}

    int keyEvent(InputEvent *event = NULL);
    int videoControl(Mat *img);


private:
    int  videoGenerate();
    bool startRecord;
    bool endRecord;

    vector<Mat> images;

};

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////landing/////////////////////////////////////////////

class FlightTask_Landing : public FlightTask_DetectMarker
{
public :
    FlightTask_Landing() : FlightTask_DetectMarker()
    {
        isMarkerFound = 0;

        rVec.create(1,3);
        tVec.create(1,3);
        rMat.create(3,3);

        detectedId.clear();
        nLostMarker  = 0;
    }

    ~FlightTask_Landing()
    {
        m_heli     = NULL;
        m_win      = NULL;
        m_rawimage = NULL;
    }

    virtual int videoControl(Mat *img);

private:
//    int solveAttitude(float &_pitch,float &_roll,float &_yaw);

    int calControlParam(vector<Marker> &Markers);
    int calControlParam(Marker _marker);

    Marker getMarkerWithLeastId(vector<Marker> &Markers);

    bool isMarkerFound;

    cv::Mat_<double> rVec;
    cv::Mat_<double> tVec;

    cv::Mat_<double> rMat;

    set<int> detectedId;
    int nLostMarker;               //record the times that marker isn't detected
};

#endif // __FLIGHTTASK_H__
