#ifndef VIDEO_H
#define VIDEO_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>
#include <linux/videodev2.h>
#include <stropts.h> //上层和底层的交互
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <QThread>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include "jpeglib.h"



struct videoBufInfo
{
    unsigned char * bufPor;
    int    buflenght;

};

class Video:public QThread
{
    Q_OBJECT

public:
    Video();

    int video_init();
    int showLcd(int * lcd , int * buf ,int x ,int y );
    int all_YUV_to_RGB(unsigned char * YUVdata,int *argBuf);
    void video_close();
    void run();
    void arrThreedTofour(int * four,unsigned char * three,int widgt ,int hight);
    void layout_temp(unsigned char *old, unsigned char *newIP,int tempheight ,int tempwidth);
    int argbTorgb(int *buf ,unsigned char *object);

    int rgbtojpg(char *rgbdata,char *jpgname);
    void MianCaller(QString picName);
private:
    int lcdfd;
    int *lcdmem;
    int vide_fd ;
    /*分配缓冲区*/
    struct v4l2_buffer allotBuf;
    /*存储分配的缓冲区的信息*/
    struct videoBufInfo videoInfobuf [4];

    unsigned char   rgbbuf[160*120*3];

};

#endif // VIDEO_H
