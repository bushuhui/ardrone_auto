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


#include <QtGui>

#include "ardrone_config.h"
#include "GUI_MainWindow.h"
#include "utils.h"


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent, unsigned char *video_buffer)
    : QWidget(parent)
{
    antialiased = true;

    // mutex for log & event
    m_mutexPrint = new QMutex(QMutex::Recursive);
    m_mutexEventQueue = new QMutex(QMutex::Recursive);

    // back-end image buffer
    m_imgVideo = new QImage(video_buffer, ARDRONE_VIDEO_WIDTH, ARDRONE_VIDEO_HEIGHT,
                            QImage::Format_RGB888);
    m_imgCpl   = new QPixmap(CONTROL_PANEL_WIDTH, CONTROL_PANEL_HEIGHT);

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    // log box
    m_txtEdit = new QPlainTextEdit(this);
    m_txtEdit->setGeometry(0, ARDRONE_VIDEO_HEIGHT+1,
                           LOG_PANEL_WIDTH, LOG_PANEL_HEIGHT);
    m_txtEdit->setFont(QFont("DejaVu Sans Mono", 10));

    m_txtEdit->show();
    m_txtEdit->setReadOnly(true);
    m_txtEdit->setWindowTitle("Log");

    // start log text-edit update timer
    startTimer(30);

    // set focus to self
    setFocus();
}

MainWindow::~MainWindow()
{
    delete m_txtEdit;
    delete m_mutexPrint;
    delete m_mutexEventQueue;
    delete m_imgVideo;
    delete m_imgCpl;
}

QSize MainWindow::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize MainWindow::sizeHint() const
{
    return QSize(800, 600);
}

int  MainWindow::getEvent(InputEvent &event)
{
    if( m_eventQueue.size() > 0 ) {
        u_int64_t       t;

        t = tm_get_millis();

        memcpy(&event, &(m_eventQueue[0]), sizeof(InputEvent));

        m_mutexEventQueue->lock();
        m_eventQueue.pop_front();
        m_mutexEventQueue->unlock();

        event.dt = t - event.timestamp;

        return 0;
    } else {
        return -1;
    }
}

void MainWindow::print(char *buf)
{
    int     len;

    m_mutexPrint->lock();

    len = strlen(buf);
    if( buf[len-1] == '\n' ) buf[len-1] = 0;

    m_arrPrintMsg.push_back(buf);

    m_mutexPrint->unlock();
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    // update log text-edit control
    m_mutexPrint->lock();
    if( m_arrPrintMsg.size() > 0 ) {
        for(int i=0; i<m_arrPrintMsg.size(); i++)
            m_txtEdit->appendPlainText(m_arrPrintMsg[i]);

        m_arrPrintMsg.clear();
    }
    m_mutexPrint->unlock();

    // update sensor information
    {
        QPainter    painter;
        QFont       fnt;
        int         w, h;
        int         x, y, dy;
        char        buf[200];

        painter.begin(m_imgCpl);
        painter.setRenderHint(QPainter::TextAntialiasing, true);


        w = m_imgCpl->width();
        h = m_imgCpl->height();

        // clear backgroud
        painter.fillRect(0, 0, w, h, QColor(255,255,255));

        // set font
        fnt.setFamily("");
        fnt.setFamily("DejaVu Sans Mono");
        fnt.setPointSize(10);
        painter.setFont(fnt);
        painter.setPen(QColor(0, 0, 0));

        // draw sensor data
        x = 20; y = 20;
        dy = 20;
        sprintf(buf, "bat: %6.2f%%", g_navData->battery);
        painter.drawText(x, y, buf);

        y += dy;
        sprintf(buf, "x, y, z = %6.2f %6.2f %6.2f",
                g_navData->x, g_navData->y, g_navData->z);
        painter.drawText(x, y, buf);

        y += dy;
        sprintf(buf, "vx, vy, vz = %6.2f %6.2f %6.2f",
                g_navData->vx, g_navData->vy, g_navData->vz);
        painter.drawText(x, y, buf);

        y += dy;
        sprintf(buf, "phi, theta, psi = %6.2f %6.2f %6.2f",
                g_navData->phi, g_navData->theta, g_navData->psi);
        painter.drawText(x, y, buf);

        y += dy;
        sprintf(buf, "altitude = %6.2f", g_navData->altitude);
        painter.drawText(x, y, buf);

        // draw seperate line
        y += 15;
        painter.setPen(QColor(0, 0, 255));
        painter.drawLine(x, y, x+CONTROL_PANEL_WIDTH-40, y);

        // draw command data
        painter.setPen(QColor(0, 0, 0));

        y += dy;
        sprintf(buf, "enable = %4f", g_cmdData->enable);
        painter.drawText(x, y, buf);

        y += dy;
        sprintf(buf, "pitch  = %4f", g_cmdData->pitch);
        painter.drawText(x, y, buf);

        y += dy;
        sprintf(buf, "roll   = %4f", g_cmdData->roll);
        painter.drawText(x, y, buf);

        y += dy;
        sprintf(buf, "gaz    = %4f", g_cmdData->gaz);
        painter.drawText(x, y, buf);

        y += dy;
        sprintf(buf, "yaw    = %4f", g_cmdData->yaw);
        painter.drawText(x, y, buf);

        painter.end();

        this->update();
    }

}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    u_int32_t   key_raw, key;
    InputEvent  ev;

    // convert Qt key code to universial key code
    key_raw = event->key();
    input_event_qt_code_trans(key_raw, key);

    ev.type        = OSA_KEY_PRESS;
    ev.code_raw    = key_raw;
    ev.code        = key;
    ev.timestamp   = tm_get_millis();

    m_mutexEventQueue->lock();
    m_eventQueue.push_back(ev);
    m_mutexEventQueue->unlock();

    // print key code
    /*
    sprintf(str_buf, "key = %8d  HEX:0x%08x  %d -> %d\n",
            event->key(), event->key(), event->key() & 0xFF,
            key);
    print(str_buf);
    */
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    u_int32_t   key_raw;
    InputEvent  ev;

    // convert Qt mouse code to universial key code
    key_raw = event->button();

    ev.timestamp   = tm_get_millis();
    ev.type        = OSA_MOUSE_DBLCLICK;
    ev.x_pos       = event->x();
    ev.y_pos       = event->y();
    if( key_raw == 1 )
        ev.code    = OSA_VK_M_LEFT;
    else if ( key_raw == 2 )
        ev.code    = OSA_VK_M_RIGHT;
    else if ( key_raw == 4 )
        ev.code    = OSA_VK_M_CENTER;

    m_mutexEventQueue->lock();
    m_eventQueue.push_back(ev);
    m_mutexEventQueue->unlock();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    u_int32_t   key_raw;
    InputEvent  ev;

    // convert Qt mouse code to universial key code
    key_raw = event->button();

    ev.timestamp   = tm_get_millis();
    ev.type        = OSA_MOUSE_PRESSE;
    ev.x_pos       = event->x();
    ev.y_pos       = event->y();
    if( key_raw == 1 )
        ev.code    = OSA_VK_M_LEFT;
    else if ( key_raw == 2 )
        ev.code    = OSA_VK_M_RIGHT;
    else if ( key_raw == 4 )
        ev.code    = OSA_VK_M_CENTER;

    m_mutexEventQueue->lock();
    m_eventQueue.push_back(ev);
    m_mutexEventQueue->unlock();

    // print mouse event messages
    /*
    sprintf(str_buf, "button = %d, x,y = (%5d, %5d)\n",
           event->button(), event->x(), event->y());
    print(str_buf);
    */
}


void MainWindow::paintEvent(QPaintEvent * /* event */)
{
    QPainter    painter(this);

    // draw video image
    painter.drawImage(QPoint(0, 0), *m_imgVideo);

    // draw nav data panel
    painter.drawPixmap(QPoint(ARDRONE_VIDEO_WIDTH+1, 0), *m_imgCpl);

    setFocus();
}
