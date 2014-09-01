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


#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QMutex>

#include <vector>
#include <deque>

#include "GUI_InputEvent.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent, unsigned char *video_buffer);
    ~MainWindow();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    int  getEvent(InputEvent &event);
    void print(char *buf);

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void timerEvent(QTimerEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    bool                    antialiased;

    QImage                  *m_imgVideo;
    QPixmap                 *m_imgCpl;

    QPlainTextEdit          *m_txtEdit;
    QMutex                  *m_mutexPrint;
    std::vector<QString>    m_arrPrintMsg;

    std::deque<InputEvent>  m_eventQueue;
    QMutex                  *m_mutexEventQueue;
};


#endif // __MAINWINDOW_H__
