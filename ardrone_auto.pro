TEMPLATE = app
CONFIG += console
#CONFIG -= app_bundle
#CONFIG -= qt

DEFINES += __STDC_CONSTANT_MACROS

INCLUDEPATH += \
    /opt/opencv-2.4.5/include/opencv /opt/opencv-2.4.5/include \
    /opt/ffmpeg-1.0/include /user/include/qt4/QtGui

LIBS += \
    -L/opt/opencv-2.4.5/lib \
    -lopencv_calib3d -lopencv_contrib -lopencv_core \
    -lopencv_features2d -lopencv_flann -lopencv_gpu \
    -lopencv_highgui -lopencv_imgproc -lopencv_legacy \
    -lopencv_ml -lopencv_nonfree -lopencv_objdetect \
    -lopencv_photo -lopencv_stitching -lopencv_ts \
    -lopencv_video -lopencv_videostab \
    -L/opt/ffmpeg-1.0/lib \
    -lavcodec -lswscale -lavutil \
    -lSDL -lz -lpthread



SOURCES += \
    ./ardrone_app.cpp \
    ./ardrone_at_cmds.cpp \
    ./ardrone_CDecoder.cpp \
    ./ardrone_CHeli.cpp \
    ./ardrone_CImageClient.cpp \
    ./GUI_InputEvent.cpp \
    ./GUI_MainWindow.cpp \
    ./cv_color_reco.cpp \
    ./utils.cpp \
    ./ardrone_CRawImage.cpp \
    ./ardrone_CRecognition.cpp \
    ./ardrone_CThread.cpp \
    ./ardrone_FlightTask.cpp \
    ./ardrone_heli.cpp \
    ./ardrone_navdata.cpp \
    ./marker/aruco_board.cpp \
    ./marker/aruco_boarddetector.cpp \
    ./marker/aruco_cameraparameters.cpp \
    ./marker/aruco_cvdrawingutils.cpp \
    ./marker/aruco_fidmarkers.cpp \
    ./marker/aruco_marker.cpp \
    ./marker/aruco_markerdetector.cpp

HEADERS += \
    ./ardrone_app.h \
    ./ardrone_CDecoder.h \
    ./ardrone_CHeli.h \
    ./GUI_InputEvent.h \
    ./GUI_MainWindow.h \
    ./cv_color_reco.h \
    ./utils.h \
    ./ardrone_config.h \
    ./ardrone_CRawImage.h \
    ./ardrone_CRecognition.h \
    ./ardrone_CThread.h \
    ./ardrone_FlightTask.h \
    ./ardrone_CImageClient.h \
    ./marker/aruco.h \
    ./marker/aruco_board.h \
    ./marker/aruco_boarddetector.h \
    ./marker/aruco_cameraparameters.h \
    ./marker/aruco_cvdrawingutils.h \
    ./marker/aruco_exports.h \
    ./marker/aruco_fidmarkers.h \
    ./marker/aruco_marker.h \
    ./marker/aruco_markerdetector.h
