#include "ardrone_CDecoder.h"
#include "ardrone_config.h"

CDecoder::CDecoder(sem_t *im,CRawImage *img)
{
	avcodec_register_all();
	av_init_packet(&avpkt);
	imageSem = im;
	codec = avcodec_find_decoder(CODEC_ID_H264);
	if (!codec) {
		fprintf(stderr, "codec not found\n");
		exit(1);
	}
	c = avcodec_alloc_context3(codec);
	picture = avcodec_alloc_frame();
	pictureRGB = avcodec_alloc_frame();
    srcX=dstX = ARDRONE_VIDEO_WIDTH;
    srcY=dstY = ARDRONE_VIDEO_HEIGHT;

    img_convert_ctx = sws_getContext(srcX, srcY, PIX_FMT_YUV420P,
                                     dstX, dstY, PIX_FMT_RGB24,
                                     SWS_BICUBIC, NULL, NULL, NULL);
	//if(codec->capabilities&CODEC_CAP_TRUNCATED) c->flags|= CODEC_FLAG_TRUNCATED; 
	if (avcodec_open2(c, codec, NULL) < 0) {
		fprintf(stderr, "could not open codec\n");
		exit(1);
	}
	image=img;
    avpicture_fill((AVPicture *)pictureRGB,image->data,PIX_FMT_RGB24, dstX, dstY);
}

CDecoder::~CDecoder()
{
	sws_freeContext(img_convert_ctx); 
	avcodec_close(c);
	av_free(c);
	av_free(picture);
}

int CDecoder::decode(unsigned char* buf,int len,int type)
{
	avpkt.size = len;
	avpkt.data = buf;
	avpkt.flags = 0;
	if (type == 1) avpkt.flags=AV_PKT_FLAG_KEY;

	avcodec_decode_video2(c, picture, &got_picture, &avpkt);
	if (got_picture > 0){
		//sem_wait(imageSem);
		sws_scale(img_convert_ctx,picture->data, picture->linesize, 0, srcY-8,pictureRGB->data,pictureRGB->linesize);
		//	sem_post(imageSem);
	}
	if (got_picture > 0) return 0;
	return 1;
}

