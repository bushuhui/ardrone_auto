/*
 * File name: CRecognition.h
 * Date:      2010
 * Author:   Tom Krajnik
 */

#ifndef __CRECOGNITION_H__
#define __CRECOGNITION_H__

#include <math.h>

#include <opencv2/core/core.hpp>

#include "ardrone_CRawImage.h"

#define MAX_SEGMENTS 10000

typedef struct{
    int x;
    int y;
    int size;
} SSegment;

typedef struct{
    int x;
    int y;
    int max_size;
} SPixelPosition;

class CRecognition
{
public:
    CRecognition();
    ~CRecognition();
    SPixelPosition findMean(CRawImage* image);
    SPixelPosition findSegment(CRawImage* image);
    SPixelPosition findSegment(Mat *image);
    void learnPixel(unsigned char* a);
    void increaseTolerance();
    void decreaseTolerance();
    void resetColorMap();

private:
    int tolerance;
    float evaluatePixel1(unsigned char* a);
    float evaluatePixel2(unsigned char* a);
    float evaluatePixel3(unsigned char* a);
    int evaluatePixelFast(unsigned char *a);
    void rgbToHsv(unsigned char r, unsigned char  g, unsigned char b, unsigned int *h, unsigned char *s, unsigned char *v );

    unsigned char learned[3];
    unsigned int learnedHue;
    unsigned char learnedSaturation,learnedValue;
    unsigned char colorArray[64*64*64];
    SSegment segmentArray[MAX_SEGMENTS];
    bool debug;
};

#endif

/* end of CRecognition.h */
