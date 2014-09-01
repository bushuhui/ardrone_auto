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

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#include <QApplication>
#include <QtGui>

#include <cv.h>
#include <highgui.h>

#include "ardrone_FlightTask.h"
#include "ardrone_CHeli.h"

#include "GUI_MainWindow.h"
#include "utils.h"

#include "ardrone_config.h"

//*************************************************************************
//thread variable
static pthread_t control_thread = 0;
static int control_live = 1;

QApplication        *g_app = NULL;
MainWindow          *g_win = NULL;

SHeliData           *g_navData;
radiogp_cmd_t       *g_cmdData;

FlightTask          *g_task;
CHeli               *g_heli;
CRawImage           *g_image;
unsigned char       *g_imgBuff;
unsigned int         g_imgBuffSize;


double marker_size = 0.2;

//****************************************************************************

static void* control_mainloop(void *arg)
{
    Mat             image_temp;

    image_temp.create(ARDRONE_VIDEO_HEIGHT, ARDRONE_VIDEO_WIDTH, CV_8UC3);

    while( control_live ) {
        //image capture
        g_heli->renewImage(g_image);

        g_image->to_mat(image_temp);

        //performing flight task
        g_task->videoControl(&image_temp);

        g_image->from_mat(&image_temp);

        //update GUI video buffer
        memcpy(g_imgBuff, g_image->data, g_imgBuffSize);

        usleep(20000);

        if( g_task->is_stopped() ) {
            g_win->close();
            break;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int print_usage(int argc, char *argv[])
{
    printf("============================ usage ============================\n");
    printf("%s mode\n", argv[0]);
    printf("    [0] ColorTrack\n");
    printf("    [1] DetectMarker\n");
    printf("    [2] VanishPoint\n");
    printf("    [3] RecordVideo\n");
    printf("    [4] Landing\n");
    printf("\n\n");

    return 0;
}

int main(int argc, char* argv[])
{
    int flight_mode = 0;

    // start stack trace
    //dbg_stacktrace_setup();

    // parse input arguments
    print_usage(argc, argv);

    if( argc > 1 ) {
        if( strcmp(argv[1], "ColorTrack") == 0 )
            flight_mode = 0;
        if( strcmp(argv[1], "DetectMarker") == 0 )
            flight_mode = 1;
        if( strcmp(argv[1], "VanishPoint") == 0 )
            flight_mode = 2;
        if( strcmp(argv[1], "RecordVideo") == 0 )
            flight_mode = 3;
        if( strcmp(argv[1], "Landing") == 0 )
            flight_mode = 4;
    }

    printf("Flight mode = %d\n\n", flight_mode);

    //set global nav & control data
    g_navData = &helidata;
    g_cmdData = &radiogp_cmd;

    ///////////////////////////////////////////////////////////////////////////
    //initializing stuff
    ///////////////////////////////////////////////////////////////////////////
    g_heli = new CHeli();
    g_image = new CRawImage(ARDRONE_VIDEO_WIDTH, ARDRONE_VIDEO_HEIGHT);
    g_image->getSaveNumber();

    ///////////////////////////////////////////////////////////////////////////
    // UI setting
    ///////////////////////////////////////////////////////////////////////////
    QApplication app(argc, argv);
    g_app = &app;

    // regist Text classes
    qRegisterMetaType<QTextBlock>("QTextBlock");
    qRegisterMetaType<QTextCursor>("QTextCursor");

    g_imgBuffSize = ARDRONE_VIDEO_WIDTH*ARDRONE_VIDEO_HEIGHT*3;
    g_imgBuff = new unsigned char[g_imgBuffSize];
    MainWindow  canvas(NULL, g_imgBuff);
    g_win = &canvas;

    int w, h;
    w = ARDRONE_VIDEO_WIDTH + CONTROL_PANEL_WIDTH + 2;
    h = ARDRONE_VIDEO_HEIGHT + LOG_PANEL_HEIGHT + 2;
    canvas.setWindowTitle("Ardrone Control");
    canvas.setGeometry(0, 0, w, h);
    canvas.setFixedSize(w, h);

    //canvas.showFullScreen();
    canvas.show();

    ///////////////////////////////////////////////////////////////////////////
    /// create new task and set flight-task parameters
    ///////////////////////////////////////////////////////////////////////////
    if( flight_mode == 0 )
        g_task = new FlightTask_ColorTrack();
    if( flight_mode == 1 )
        g_task = new FlightTask_DetectMarker();
    if( flight_mode == 2 )
        g_task = new FlightTask_VanishPoint();
    if( flight_mode == 3 )
        g_task = new FlightTask_RecordVideo();
    if( flight_mode == 4 )
        g_task = new FlightTask_Landing();

    g_task->set_heli(g_heli);
    g_task->set_rawimage(g_image);
    g_task->set_gui(g_win);

    if( flight_mode == 1 ) {
        FlightTask_DetectMarker *t = (FlightTask_DetectMarker*) g_task;

        t->setMarkersize(marker_size);
        t->setCameraParameters("intrinsics.yml");
    }

    ///////////////////////////////////////////////////////////////////////////
    /// create control loop
    ///////////////////////////////////////////////////////////////////////////
    control_live = 1;
    if ( pthread_create( &control_thread, NULL, control_mainloop, NULL ) ) {
        printf("ERR: pthread_create: %s\n", strerror(errno));
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// begin Qt GUI loop
    ///////////////////////////////////////////////////////////////////////////
    g_app->exec();

    ///////////////////////////////////////////////////////////////////////////
    /// terminate control loop
    ///////////////////////////////////////////////////////////////////////////
    control_live = 0;
    pthread_join(control_thread, NULL);

    delete g_heli;
    delete g_image;
    delete g_task;
    delete g_imgBuff;

    return 0;
}

