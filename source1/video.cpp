#include "video.h"


#include <QDebug>
/*构造函数*/
Video::Video()
{
    /*进行摄像头初始化*/
    this->video_init();
}


//封装一个函数把一组YUV数据转换成RGB
int YUVtoRGB(int y,int u,int v )
{
    /*三原色*/
    int r,g,b;
    int pix; //保存一个像素点的ARGB数据
    //算法1
    // r = 1164*(y-16)/1000 + 1596*(v-128)/1000;
    // g = 1164*(y-16)/1000 + 813*(v-128)/1000 - 391*(u-128)/1000;
    // b = 1164*(y-16)/1000 + 2018*(u-128)/1000;

    //算法2
    // r= 1.164383 * (y - 16) + 1.596027*(v - 128);
    // g= 1.164383 * (y - 16) - 0.391762*(u - 128) - 0.812968*(v- 128);
    // b = (1.164383 * (y - 16) + 2.017232*(u - 128));

    //算法3
    // r= y + ((360 * (v - 128))>>8) ;
    // g= y - ( ( ( 88 * (u - 128)  + 184 * (v - 128)) )>>8) ;
    // b= y + ((455 * (u - 128))>>8) ;

    //算法4
    // r = 1.164*y + 1.596 * v - 222.9;
    // g = 1.164*y - 0.392 * u - 0.823 * v+ 135.6;
    // b = 1.164*y + 2.017 * u- 276.8 ;

    //算法5
    r = (298*y + 411 * v - 57344)>>8;
    g = (298*y - 101* u - 211* v+ 34739)>>8;
    b = (298*y + 519* u- 71117)>>8;

    /*修正画面*/
    if (r > 255)
    {
        r = 255;
    }
    if (g > 255)
    {
        g = 255;
    }
    if (b > 255)
    {
        b = 255;
    }

    if (r < 0)
    {
        r = 0;
    }
    if (g < 0)
    {
        g = 0;
    }
    if (b < 0)
    {
        b = 0;
    }
    /*像素点拼接*/
    pix=0x00<<24|r<<16|g<<8|b;

    /*返回argb的像素点*/
    return pix;
}

/*保存图片*/
void Video::MianCaller(QString picName)
{
    /*转换的容器*/
    unsigned char tempbuf[640*480*3];
    /*进行像素转换*/
    this->layout_temp(this->rgbbuf,tempbuf,480,640);
    /*保存成jpg图片*/
    this->rgbtojpg((char*)tempbuf,picName.toLatin1().data());
}

//封装函数，实现把RGB数据保存成jpg图片
int Video::rgbtojpg(char *rgbdata1,char *jpgname)
{
    int i;
    //定义压缩结构体变量和错误处理变量并初始化
    struct jpeg_compress_struct mycom;
    jpeg_create_compress(&mycom);
    struct jpeg_error_mgr myerr;
    mycom.err=jpeg_std_error(&myerr);

    //设置压缩参数
    mycom.image_width=640;
    mycom.image_height=480;
    mycom.input_components=3;
    mycom.in_color_space=JCS_RGB; //RGB格式
    jpeg_set_defaults(&mycom);

    //设置压缩比例
    jpeg_set_quality(&mycom,90,true);

    //新建空白的jpg文件
    FILE *myfile=fopen(jpgname,"wb");
    if(myfile==NULL)
    {
        perror("新建jpg失败！");
        return -1;
    }

    //绑定输出
    jpeg_stdio_dest(&mycom,myfile);

    //开始压缩
    jpeg_start_compress(&mycom,true);

    //定义数组存放要写入jpg中的数据
    JSAMPROW array[1];

    //把压缩后的数据写入到空白的jpg文件中
    /*
        typedef JSAMPLE FAR *JSAMPROW;
        typedef JSAMPROW *JSAMPARRAY;

        rgbdata[0]----rgbdata[W-1]
        rgbdata[W]----rgbdata[2*W-1]
    */
    for(i=0; i<480; i++)
    {

        array[0]=(unsigned char *)rgbdata1+i*640*3;
        jpeg_write_scanlines(&mycom,array,1);
    }

    //收尾
    jpeg_finish_compress(&mycom);
    jpeg_destroy_compress(&mycom);
    fclose(myfile);
    printf("%s 保存成功\n",jpgname);
    return 0;
}

/*一帧的转换*/
int Video::all_YUV_to_RGB(unsigned char * YUVdata,int *argBuf)
{

    for(int i=0,j=0; j < 120*160; i+=4,j+=2)
    {
        //一组YUYV转换成两组ARGB
        argBuf[j]=YUVtoRGB(YUVdata[i],YUVdata[i+1],YUVdata[i+3]);
        argBuf[j+1]=YUVtoRGB(YUVdata[i+2],YUVdata[i+1],YUVdata[i+3]);

    }
    return 0;
}

/*显示在屏幕 后两位为坐标*/
int Video::showLcd(int * lcd , int * buf ,int x,int y )
{

    for (int i = 0; i < 120 ; i++)
    {
       for (int j = 0; j < 160; j++)
       {
           lcd[j + x + (i+y)*(800)] = buf[j + i*(160)];
       }

    }

       return  0;
}

/*摄像头初始化*/
int Video::video_init()
{
    /*打开驱动文件*/
    this->vide_fd= open("/dev/video7",O_RDWR);
    if (this->vide_fd < 0)
    {
        perror("open video failer");
        return 0;
    }

    /*打开液晶屏文件*/
    this->lcdfd=open("/dev/fb0",O_RDWR);
    if(lcdfd==-1)
    {
        perror("open lcd file failer");
        return -1;
    }

    /*映射液晶屏*/
    this->lcdmem=(int *)mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,this->lcdfd,0);
    if(lcdmem==NULL)
    {
        perror("mmap lcd file failer");
        return -1;
    }

    /*设置摄像头的采集格式*/
    /*采集格式结构体*/
    struct v4l2_format Myfmt;

    /*清空采集格式结构体*/
    bzero(&Myfmt,sizeof(Myfmt));

    /*设置采集格式类型*/
    Myfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    /*设置采集的画面宽*/
    Myfmt.fmt.pix.width = 160;
    /*设置采集的画面高*/
    Myfmt.fmt.pix.height = 120;
    /*设置采集的画面像素点的格式*/
    Myfmt.fmt.pix.pixelformat=V4L2_PIX_FMT_YUYV;

    /*设置采集格式*/
    int ret = ioctl(vide_fd,VIDIOC_S_FMT,&Myfmt);
    if(ret  == -1)
    {
        perror("set video failer:");
        return 0;
    }


    /*申请摄像头的缓冲区*/
    struct v4l2_requestbuffers requesBuf;
    /*清空结构体*/
    bzero(&requesBuf,sizeof(struct v4l2_requestbuffers));
    /*设置结构体成员*/
    requesBuf.count = 4;    //缓冲块数量

    /*Stream 或者Buffer的类型*/
    requesBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /*Memory Mapping模式*/
    requesBuf.memory = V4L2_MEMORY_MMAP;

    /*申请缓冲区命令*/
    ret = ioctl(vide_fd,VIDIOC_REQBUFS,&requesBuf);
    if(ret  == -1)
    {
        perror("reques buf failer:");
        return 0;
    }




    for (int i = 0; i < 4; i++)
    {
        /*清空缓冲区*/
        bzero(&allotBuf,sizeof(struct v4l2_buffer));

        /*缓冲区下标*/
        allotBuf.index = i;

        /*缓冲区类型*/
        allotBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        /*缓冲区存储*/
        allotBuf.memory = V4L2_MEMORY_MMAP;

        /*向驱动发送分配指令*/
        ret = ioctl(vide_fd,VIDIOC_QUERYBUF,&allotBuf);
        if(ret  == -1)
        {
            perror("allot buf failer:");
            return 0;
        }

        /*获取其映射的首地址*/
                                //自动分配 缓冲区的大小   可读可写的权限       可共享     文件描述符 地址偏移
        videoInfobuf[i].bufPor = (unsigned char*)mmap(NULL,allotBuf.length,PROT_READ|PROT_WRITE, MAP_SHARED,vide_fd, allotBuf.m.offset);
        if ( videoInfobuf[i].bufPor == NULL)
        {
            perror("mmap video buf failer");
            return 0;
        }

        /*获取地址的大小*/
        videoInfobuf[i].buflenght = allotBuf.length;

        /*画面提前入队*/
        ret = ioctl(vide_fd,VIDIOC_QBUF,&allotBuf);
        if(ret  == -1)
        {
            perror(" ahead join array failer");
            return 0;
        }
    }


    /*开启摄像头*/
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(vide_fd,VIDIOC_STREAMON,&type);
    if(ret  == -1)
    {
        perror(" open video failer:");
        return 0;
    }

    printf("摄像头开启成功，初始化ok\n");

    return 0;
}

/*摄像头关闭*/
void Video::video_close()
{
    /*关闭文件,取消映射*/
    munmap(lcdmem, 800*480*4);
    close(vide_fd);
    close(lcdfd);

}

void Video::run()
{
    /*定义存放ARGB数据的数组*/
    int argbBuf[160*120];

    /*视频流*/
    while(1)
    {
        for(int i=0; i<4; i++)
        {
             /*清空缓冲区*/
            bzero(&allotBuf,sizeof(struct v4l2_buffer));

            /*缓冲区下标*/
            allotBuf.index = i;

            /*缓冲区类型*/
            allotBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

            /*缓冲区存储*/
            allotBuf.memory = V4L2_MEMORY_MMAP;


            /*画面出队*/
            int ret=ioctl(vide_fd,VIDIOC_DQBUF,&allotBuf);
            if(ret==-1)
            {
                perror("out array failer:");
                return ;
            }
            /*画面入队*/
            ret=ioctl(vide_fd,VIDIOC_QBUF,&allotBuf);
            if(ret==-1)
            {
                perror("join array failer");
                return ;
            }


            /*转换存储*/
            all_YUV_to_RGB(videoInfobuf[i].bufPor,argbBuf);

            /*argb 转换成 rgb*/
            argbTorgb(argbBuf,rgbbuf);

            /*显示*/
            showLcd(lcdmem,argbBuf,620,265  );
        }
    }
}

/*rgb  转成 argb*/
void Video::arrThreedTofour(int *four, unsigned char *three ,int widgt1 ,int hight1)
{
    for(int i = 0 ; i< hight1 ;i++)
    {
        for(int j = 0; j<widgt1 ;j++)
        {
            four[i*widgt1 + j] = 0x00<<24 |three[i*widgt1*3+j*3 +0]<<16 | three[i*widgt1*3+j*3 +1]<<8 |three[i*widgt1*3+j*3 +2];
        }
    }

}




/*转换*/
void Video::layout_temp(unsigned char *old, unsigned char *newIP,int tempheight ,int tempwidth)
{

    /*长宽转换*/
    unsigned long dwsrcX, dwsrcY;
    unsigned char * pucDest ;
    unsigned char * pucSrc ;
    //unsigned char * new_data = (unsigned char *)calloc(1 , 800*480*3);

    for (int i = 0; i < tempheight; i++)
    {
        /*获取高的倍数*/
        dwsrcY = i* 120 / tempheight;

        /*获取新地点每行的地址*/
        pucDest = newIP + i * tempwidth *3;

        /*获取旧数据每行的地址*/
        pucSrc = old + dwsrcY * 160 *3;
        for (int j = 0; j < tempwidth; j++)
        {
            /*获取宽的倍数*/
            dwsrcX = j* 160 / tempwidth;

            /*将等量倍数的像素点写入新的地址中*/
            memcpy(pucDest + j * 3 , pucSrc + dwsrcX *3 , 3);
        }

    }
}

/*int 的argb的像素点 装换成 rgb的char型*/
int Video::argbTorgb(int *buf ,unsigned char *object)
{
    for (int i = 0; i < 120; i++)
    {
        for (int j = 0; j < 160; j++)
        {
            object[i*160*3 + j*3+0] =(char)( buf[i*160+j] >>16) ;
            object[i*160*3 + j*3+1] =(char)( buf[i*160+j] >>8)  ;
            object[i*160*3 + j*3+2] =(char) buf[i*160+j] ;
        }

    }


    return  0;
}



