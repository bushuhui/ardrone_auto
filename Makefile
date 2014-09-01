################################################################################
# Compiler settings
################################################################################
CC  = gcc
CXX = g++
MOC = moc-qt4

################################################################################
# OpenCV
# run the following command first if the OpenCV install can not be found: 
#   export PKG_CONFIG_PATH=/opt/OpenCV-2.4/lib/pkgconfig
################################################################################
OPENCV_CFLAGS = $(shell pkg-config --cflags opencv)
OPENCV_LIBS   = $(shell pkg-config --libs opencv)

################################################################################
# OpenGL settings
################################################################################
OPENGL_CFLAGS = 
OPENGL_LIBS   = -lGL -lGLU -lglut

################################################################################
# Qt settings
################################################################################
QT_CFLAGS = -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED \
            -I/usr/share/qt4/mkspecs/linux-g++ \
            -I/usr/include/qt4/QtCore \
            -I/usr/include/qt4/QtGui \
            -I/usr/include/qt4
QT_LIBS   = -lQtGui -lQtCore 


################################################################################
# ffmpeg
# install dir: /opt/ffmpeg-1.0 or use system (/usr)
################################################################################
#FFMPEG_DIR    = /opt/ffmpeg-1.0
FFMPEG_DIR    = ./usr
FFMPEG_CFLAGS = -I$(FFMPEG_DIR)/include
FFMPEG_LIBS   = -L$(FFMPEG_DIR)/lib -lavcodec -lswscale -lavutil 


################################################################################
# libaruco
################################################################################
LIBARUCO_CFLAGS = -I./marker
LIBARUCO_LIBS   = -L./marker -laruco

################################################################################
# other
################################################################################
OTHER_CFLAGS = -D__STDC_CONSTANT_MACROS
OTHER_LIBS   = -lz -lpthread


################################################################################
# overall CFLAGS & LDFLAGS
################################################################################
CXXFLAGS   = $(OPENCV_CFLAGS) $(FFMPEG_CFLAGS) $(LIBARUCO_CFLAGS) $(OPENGL_CFLAGS) $(QT_CFLAGS) $(OTHER_CFLAGS)
LDFLAGS    = $(LIBARUCO_LIBS) $(FFMPEG_LIBS) $(OPENCV_LIBS) $(OPENGL_LIBS) $(QT_LIBS) $(OTHER_LIBS)
MOC_CFLAGS = $(QT_CFLAGS)

#CXXFLAGS += -g -rdynamic


################################################################################

################################################################################
src-all := $(wildcard *.cpp)
obj-all := $(patsubst %.cpp,%.o,$(src-all))
inc-all := $(wildcard *.h)

target_req = ardrone_app.o ardrone_at_cmds.o ardrone_CDecoder.o ardrone_CHeli.o ardrone_CImageClient.o \
			cv_color_reco.o ardrone_CRawImage.o ardrone_CRecognition.o ardrone_CThread.o  \
			ardrone_FlightTask.o ardrone_heli.o ardrone_navdata.o  \
			GUI_MainWindow.o GUI_moc_MainWindow.o utils.o GUI_InputEvent.o  


all : ardrone_auto.e

GUI_moc_MainWindow.o : GUI_MainWindow.h
	$(MOC) GUI_MainWindow.h -o GUI_moc_MainWindow.cpp $(MOC_CFLAGS)
	$(CXX) -c GUI_moc_MainWindow.cpp -o GUI_moc_MainWindow.o $(CXXFLAGS)

ardrone_auto.e : $(target_req)
	$(CXX) -o $@ $? $(LDFLAGS) 

all : $(target)


%.e:%.cpp $(inc-all)
	$(CXX) $< -o $@

%.o:%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean :
	rm -f *.e *.o



