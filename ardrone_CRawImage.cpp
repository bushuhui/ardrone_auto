#include "ardrone_CRawImage.h"

static unsigned char header[] =
{
    66,77,54,16,14,0,0,0,0,0,54,0,0,0,40,0,0,0,128,2,0,0,224,1,0,0,
    1,0,24,0,0,0,0,0,0,16,14,0,18,11,0,0,18,11,0,0,0,0,0,0,0,0,0,0
};

CRawImage::CRawImage(int w,int h)
{
    width = w;
    height = h;
    bpp= 3;
    size = bpp*width*height;
    data = (unsigned char*)calloc(size,sizeof(unsigned char));
    header[18] = width%256;
    header[19] = width/256;
    header[22] = height%256;
    header[23] = height/256;
    numSaved = 0;
}

int CRawImage::getSaveNumber()
{
    char name[100];
    FILE* file = NULL;
    do{
        sprintf(name,"%04i.bmp",numSaved++);
        file = fopen(name,"r");
    } while (file != NULL);
    numSaved--;
    return numSaved;
}

CRawImage::~CRawImage()
{
    free(data);
    data = NULL;
}

void CRawImage::swap()
{
    unsigned char* newData = (unsigned char*)calloc(size,sizeof(unsigned char));
    int span = width*bpp;
    for (int j = 0;j<height;j++){
        memcpy(&newData[span*j],&data[span*(height-1-j)],span);
        for (int i = 0;i<width;i++){
            char a = newData[(width*j+i)*3];
            newData[(width*j+i)*3] = newData[(width*j+i)*3+2];
            newData[(width*j+i)*3+2] = a;
        }
    }
    memcpy(data,newData,size);
    free(newData);
}

void CRawImage::saveBmp(const char* inName)
{
    FILE* file = fopen(inName,"wb");
    swap();
    fwrite(header,54,1,file);
    fwrite(data,size,1,file);
    swap();
    fclose(file);
}

void CRawImage::saveBmp()
{
    char name[100];
    sprintf(name,"%04i.bmp",numSaved++);
    saveBmp(name);
}


bool CRawImage::loadBmp(const char* inName)
{
    FILE* file = fopen(inName,"rb");
    if (file!=NULL)
    {
        fread(data,54,1,file);
        bpp = 3;
        size = height*width*bpp;
        fread(data,size,1,file);
        fclose(file);
        swap();
        return true;
    }
    return false;
}

// convert to/from OpenCV Mat
int CRawImage::to_mat(Mat &img)
{
    int             i, j;
    unsigned char   *ps, *pd;

    // check image buffer
    if( data == NULL )
        return -1;

    // create OpenCV Mat if necessary
    if( img.cols != width || img.rows != height || img.channels() != 3 ) {
        //img.release();
        img.create(height, width, CV_8UC3);
    }

    // get image buffer
    pd = img.data;
    ps = data;

    // copy image buffer
    for(i=0; i<height*width; i++) {
        pd[0] = ps[2];
        pd[1] = ps[1];
        pd[2] = ps[0];

        ps += 3;
        pd += 3;
    }

    return 0;
}

bool CRawImage::from_mat(Mat *cv_img)
{
    int             w, h, c;
    unsigned char   *ps, *pd;
    int             i;

    // get input image size, format
    w = cv_img->cols;
    h = cv_img->rows;
    c = cv_img->channels();

    // get image buffer
    pd = data;
    ps = cv_img->data;

    // copy image buffer
    if( c == 3 ) {
        for(i=0; i<height*width; i++) {
            pd[0] = ps[2];
            pd[1] = ps[1];
            pd[2] = ps[0];

            ps += 3;
            pd += 3;
        }
    } else if ( c == 1 ) {
        for(i=0; i<w*h; i++) {
            pd[0] = ps[0];
            pd[1] = ps[0];
            pd[2] = ps[0];

            pd += 3;
            ps += 1;
        }
    } else {
        printf("ERR: Unsupported cv Mat format!\n");
        return false;
    }

    return true;
}

void CRawImage::plotCenter()
{
    int centerWidth = 20;
    unsigned char color[] = {255,150,150};
    for (int i = -centerWidth;i<centerWidth;i++){
        for (int j =0;j<3;j++){
            data[(width*(height/2+i)+width/2-centerWidth)*3+j] = color[j];
            data[(width*(height/2+i)+width/2+centerWidth)*3+j] = color[j];
            data[(width*(height/2-centerWidth)+width/2+i)*3+j] = color[j];
            data[(width*(height/2+centerWidth)+width/2+i)*3+j] = color[j];
        }
    }
}

void CRawImage::plotLine(int x,int y) {
    int base;
    if (y < 0 || y > height-1) y = height/2;
    if (x < 0 || x > width-1) x = width/2;
    for(int i=0; i < width;i++) {
        if (i == width/2) i++;
        base = (width*y+i)*3;
        data[base+0] = 255;
        data[base+1] = 0;
        data[base+2] = 255;
    }

    for(int j=0;j<height;j++) {
        const int bidx = ((width*j)+x)*3;
        if (j == height/2) j++;
        data[bidx+0] = 255;
        data[bidx+1] = 255;
        data[bidx+2] = 0;
    }
}
