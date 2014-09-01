#include "ardrone_CHeli.h"
#include "ardrone_config.h"

float saturation(float a,float b)
{
    if (a > b) return b;
    if (a < -b) return -b;
    return a;
}

CHeli::CHeli()
{
    landed = true;

    appInit();

    imageWidth = ARDRONE_VIDEO_WIDTH;
    imageHeight = ARDRONE_VIDEO_HEIGHT;

    //	sem_init(&imageSem,0,1);
    client = new CImageClient(&imageSem);
    client->connectServer(WIFI_MYKONOS_IP,"5555");
    image = new CRawImage(imageWidth,imageHeight);
    client->run(image);
}

CHeli::~CHeli()
{
    delete client;
    appDeinit();
}

void CHeli::takeoff()
{
    if (landed){
        usleep(100000);
        at_ui_reset();
        usleep(200000);
        at_trim();
        usleep(200000);
        fprintf(stdout,"Taking off");
        at_ui_pad_start_pressed();
        usleep(100000);
        at_comwdg();
        landed = false;
    }
}

void CHeli::switchCamera(int cam){
    at_zap(cam);
}

void CHeli::land()
{
    if (landed ==false){
        usleep(100000);
        at_ui_pad_start_pressed();
        usleep(100000);
        landed = true;
    }
}

void CHeli::close()
{
    appDeinit();
}

int CHeli::renewImage(CRawImage* im)
{
    //	sem_wait(&imageSem);
    memcpy(im->data,client->image->data,im->size);
    //	sem_post(&imageSem);
    return 0;
}

void CHeli::setAngles(float ipitch, float iroll,float iyaw,float iheight)
{
    int32_t _yaw,_pitch,_roll,_height;

    _yaw    = saturation(iyaw,   33000);
    _roll   = saturation(iroll,  33000);
    _pitch  = saturation(ipitch, 33000);
    _height = saturation(iheight,33000);

    //	fprintf(stdout,"Angle request: %d %d %d %d ",pitch,roll,height,yaw);
    at_set_radiogp_input(_roll, _pitch, _height, _yaw);
}	
