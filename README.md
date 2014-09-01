# ArDrone Automatic Flight Toolkit

This program is a automatic flight toolkit for ArDrone 2. We implenented 5 flight tasks. They are [0] tracking color block, [1] tracking Aruco marker (http://www.uco.es/investiga/grupos/ava/node/26) [2] Flight along corridor 
, [3] Recorder video, [4] Auto landing.


The source is based on Tomas KRAJNIK's source code (http://labe.felk.cvut.cz/~tkrajnik/ardrone/), and Aruco (http://www.uco.es/investiga/grupos/ava/node/26). We fix some bugs, and change the SDL to Qt GUI. 


## Requirements:
* Aruco ( included)
* Qt4 (sudo apt-get install libqt4-core libqt4-dev)
* OpenCV (sudo apt-get install libcv2.4)
* ffmpeg (sudo apt-get install ffmpeg libavcodec53 libavcodec-dev libswscale2 libswscale-dev libavutil-dev libavutil51)


## Compile:
```
cd marker
make
cd ..
make
```


## Usage:
```
./ardrone_auto.e mode
    [0] ColorTrack
    [1] DetectMarker
    [2] VanishPoint
    [3] RecordVideo
    [4] Landing
```


examples:

`./ardrone_auto.e 0` (Tracking a color block)

`./ardrone_auto.e 3` (Record video to file)


keyboard key:
```
[Enter]         take off / land
[Space]         auto-flight / manually
[a]             yaw -
[d]             yaw +
[Left]          roll -
[Right]         roll +
[Up]            pitch -
[Down]          pitch +
[s]             hight +
[w]             hight -
[z]             camera 0
[x]             camera 1
[p]             screen capture
```

## Plateform:
Only test on Linux Mint 16 64-bit. 


## Issues:
* If can not connect to ArDrone2, please check 'ardone_CImageClient.cpp', change headerSize = 64 in function CimageClient::DoExecute()


## Screenshot:
-![alt text](https://raw.githubusercontent.com/bushuhui/ardrone_auto/master/images/pic_1.png "Screenshot 1")
-![alt text](https://raw.githubusercontent.com/bushuhui/ardrone_auto/master/images/pic_2.png "Screenshot 2")


## Project homepage:
http://www.adv-ci.com/blog/project/ardrone_auto/
