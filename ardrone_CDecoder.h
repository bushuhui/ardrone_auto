#ifndef CDECODER_H
#define CDECODER_H

#include <math.h>
#include <semaphore.h>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

#include "ardrone_CRawImage.h"

#define INBUF_SIZE 4096

typedef struct {
    uint8_t signature[4]; /* "PaVE" - used to identify the start of
                 frame */
    uint8_t version; /* Version code */
    uint8_t video_codec; /* Codec of the following frame */
    uint16_t header_size; /* Size of the parrot_video_encapsulation_t
                 */
    uint32_t payload_size; /* Amount of data following this PaVE */
    uint16_t encoded_stream_width; /* ex: 640 */
    uint16_t encoded_stream_height; /* ex: 368 */
    uint16_t display_width; /* ex: 640 */
    uint16_t display_height; /* ex: 360 */
    uint32_t frame_number; /* Frame position inside the current stream
                  */
    uint32_t timestamp; /* In milliseconds */
    uint8_t total_chuncks; /* Number of UDP packets containing the
                  current decodable payload - currently unused */
    uint8_t chunck_index ; /* Position of the packet - first chunk is #0
                  - currenty unused*/
    uint8_t frame_type; /* I-frame, P-frame -
                   parrot_video_encapsulation_frametypes_t */
    uint8_t control; /* Special commands like end-of-stream or
                advertised frames */
    uint32_t stream_byte_position_lw; /* Byte position of the current payload in
                         the encoded stream - lower 32-bit word */
    uint32_t stream_byte_position_uw; /* Byte position of the current payload in
                         the encoded stream - upper 32-bit word */
    uint16_t stream_id; /* This ID indentifies packets that should be
                   recorded together */
    uint8_t total_slices; /* number of slices composing the current
                 frame */
    uint8_t slice_index ; /* position of the current slice in the frame
                 */
    uint8_t header1_size; /* H.264 only : size of SPS inside payload -
                 no SPS present if value is zero */
    uint8_t header2_size; /* H.264 only : size of PPS inside payload -
                 no PPS present if value is zero */
    uint8_t reserved2[2]; /* Padding to align on 48 bytes */
    uint32_t advertised_size; /* Size of frames announced as advertised
                     frames */
    uint8_t reserved3[12]; /* Padding to align on 64 bytes */
} __attribute__ ((packed)) parrot_video_encapsulation_t;



/**
@author Tom Krajnik
*/
class CDecoder
{
public:
    CDecoder(sem_t *im,CRawImage *image);
    ~CDecoder();

    int decode(unsigned char* buf,int len,int type);

private:
    AVCodec *codec;
    AVCodecContext *c;
    int frame, got_picture, len;
    AVFrame *picture;
    CRawImage *image;
    AVFrame *pictureRGB;
    uint8_t inbuf[INBUF_SIZE];
    AVPacket avpkt;
    struct SwsContext *img_convert_ctx;
    int srcX,srcY,dstX,dstY;
    sem_t *imageSem;
};

#endif
