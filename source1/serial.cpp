#include "serial.h"


/*构造函数*/
serial::serial()
{
    /*进行初始*/
    this->serial_mian(buf);

}

volatile unsigned int cardid ;
/*超时*/
struct timeval timeout;

/*串口配置*/
int serial::serial_init(int fd)
{
    /*串口结构体*/
    struct termios  termios_new;

    /*清空结构体*/
    bzero(&termios_new,sizeof(struct termios));

    /*cfmakeraw()设置终端属性，就是设置termios结构中的各个参数。*/
    cfmakeraw(&termios_new);

    /*设置波特率*/
    cfsetispeed(&termios_new, B9600);
    cfsetospeed(&termios_new, B9600);

    /*设置模式*/
    termios_new.c_cflag |= CLOCAL |CREAD;

    /*设置数据位*/
    termios_new.c_cflag &= ~CSIZE;
    termios_new.c_cflag |= CS8;

    /*设置无奇偶校验位*/
    termios_new.c_cflag &= ~PARENB;

    /*一位停止位*/
    termios_new.c_cflag &= ~CSTOPB;

    /*设置接收字符和等待时间*/
    termios_new.c_cc[VTIME] = 2;
    termios_new.c_cc[VMIN]  = 1;

    /*清空输入/输出缓冲区*/
    tcflush(fd,TCIFLUSH);

    /*激活*/
    tcsetattr(fd,TCSANOW,&termios_new);

    return 0;
}

/*计算校验和*/
unsigned char serial::getBCC(unsigned char *buf, int n)
{
    int i;
    unsigned char bcc=0;
    for(i = 0; i < n; i++)
    {
        bcc ^= *(buf+i);
    }
    return (~bcc);
}

/*发送A命令*/
int serial::send_A_order(int fd)
{
    //发送A命名
    unsigned char send_buf[54] , server_buf[54];

    /*清空*/
    bzero(send_buf,sizeof(send_buf));
    bzero(server_buf,sizeof(server_buf));

    send_buf[0] = 0x07; //帧长 7位
    send_buf[1] = 0x02; //包号
    send_buf[2] = 0x41; //命令'A'
    send_buf[3] = 0x01; //信号长度
    send_buf[4] = 0x52; //请求模式 : ALL = 0x52
    send_buf[5] = this->getBCC(send_buf,send_buf[0]-2);
    send_buf[6] = 0x03; // 结束

    fd_set rdfd;
    int len = 0;
    while (1)
    {
        FD_ZERO(&rdfd);
        FD_SET(fd,&rdfd);
        tcflush (fd, TCIFLUSH);
        //把A命令发送给RFID读取模块
        write(fd, send_buf, 7);


        int ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);  //监测文件描述符的变化
        switch(ret)
        {
            case -1:
                perror("select error\n");
                break;
            case  0: //超时
                len++;   //3次请求超时后，退出该函数
                if(len == 3)
                {
                    len=0;
                    return -1;
                }

                //printf("Request timed out.\n");
                break;
            default:
                usleep(10000);  //微秒
                ret = read(fd, server_buf, 8);

                if(ret < 0)
                {

                    break;
                }

                //printf("RBuf[2]:%x\n", RBuf[2]);
                if(server_buf[2] == 0x00)	 	//应答帧状态部分为0 则请求成功
                {
                    return 0;
                }
                break;
        }

        usleep(100000);
    }

    return -1;
}

/*发送B命令*/
int serial::send_B_older(int fd)
{
        //printf("fd = %d\n", fd);

    unsigned char send_buf[54], server_buf[54];
    int  len=0;
    fd_set rdfd;

     /*清空*/
    bzero(send_buf,sizeof(send_buf));
    bzero(server_buf,sizeof(server_buf));

    send_buf[0] = 0x08;	//帧长= 8 Byte
    send_buf[1] = 0x02;	//包号= 0 , 命令类型= 0x01
    send_buf[2] = 0x42;	//命令= 'B'
    send_buf[3] = 0x02;	//信息长度= 2
    send_buf[4] = 0x93;	//防碰撞0x93
    send_buf[5] = 0x00;	//位计数0
    send_buf[6] =  this->getBCC(send_buf, send_buf[0]-2);		//校验和
    send_buf[7] = 0x03; 	//结束标志

     int ret = 1;
    while(1)
    {
        tcflush (fd, TCIFLUSH);
        FD_ZERO(&rdfd);
        FD_SET(fd,&rdfd);

        /*发送命令*/
        write(fd, send_buf, 8);

        /*多路复用*/
        ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);

        switch(ret)
        {
            case -1:
                perror("select error\n");
                break;
            case  0:
                len++;
                if(len == 10)
                {
                    len=0;
                    return -1;
                }
                perror("Timeout:");
                break;

            default:
                usleep(10000);
                ret = read(fd, server_buf, 10);
                if (ret < 0)
                {
                    printf("ret = %d\n", ret);
                    break;
                }
                if (server_buf[2] == 0x00) //应答帧状态部分为0 则获取ID 成功
                {
                    cardid = (server_buf[7]<<24) | (server_buf[6]<<16) | (server_buf[5]<<8) | server_buf[4];

                    return 0;
                }
        }

    }

    return -1;
}

/*串口主函数*/
int serial::serial_mian(char * serial_drive)
{
    /*打开串口驱动*/
    this->serial_fd= open(serial_drive,O_RDWR |O_NOCTTY | O_NONBLOCK);
    if (serial_fd == -1)
    {
        perror("open serial drive failer");
        return -1;
    }

    /*配置串口*/
    serial_init(serial_fd);

    /*配置超时*/
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;


    return 0;


}

void serial::run()
{
    /*发送命令*/
    while (1)
    {
        int ret = send_A_order(serial_fd);
        if (ret == -1 )
        {
            //延时
            usleep(500000);
            close(serial_fd); //关闭文件
            this->serial_mian(buf);
            continue;
        }

        /*A命令发送成功 ,发送B命令*/
        ret = send_B_older(serial_fd);

        //若获取的cardid为0，或获取id超时，则需重新发送'A'命令
        if(cardid == 0 || ret == -1)
        {
            continue;
        }
        else if(ret == 0)
        {
            printf("card ID = %x\n", cardid);  //打印cardid号

            emit this->returnCad(cardid);

            usleep(500000);
            //break;
        }

    }
}

