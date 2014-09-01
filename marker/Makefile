CC  = gcc
CXX = g++
AR  = ar

# run: export PKG_CONFIG_PATH=/opt/OpenCV-2.4/lib/pkgconfig
OPENCV_CFLAGS = $(shell pkg-config --cflags opencv)
OPENCV_LIBS   = $(shell pkg-config --libs opencv)

src-all := $(wildcard *.cpp)
obj-all := $(patsubst %.cpp,%.o,$(src-all))
inc-all := $(wildcard *.h)
bin-all := $(patsubst %.cpp,%.e,$(src-all))

target  := libaruco.a


all : $(target)

$(target) : $(obj-all) $(inc-all)
	$(AR) rcs $@ $(obj-all) 

%.e:%.cpp $(inc-y)
	$(CXX) $< -o $@ $(OPENCV_CFLAGS) $(OPENCV_LIBS) -lm

%.o:%.cpp
	$(CXX) -c $< -o $@ $(OPENCV_CFLAGS)

clean :
	rm -f *.e *.o *.a

