#ifndef ARDRONE_CONFIG_H
#define ARDRONE_CONFIG_H

#define ARDRONE_VIDEO_WIDTH     640
#define ARDRONE_VIDEO_HEIGHT    368

#define LOG_PANEL_WIDTH         640
#define LOG_PANEL_HEIGHT        225

#define CONTROL_PANEL_WIDTH     350
#define CONTROL_PANEL_HEIGHT    (ARDRONE_VIDEO_HEIGHT+ARDRONE_VIDEO_HEIGHT+1)

#include "ardrone_app.h"

// global data
extern SHeliData         *g_navData;
extern radiogp_cmd_t     *g_cmdData;


#endif // ARDRONE_CONFIG_H
