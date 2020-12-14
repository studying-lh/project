#ifndef SERIAL_H
#define SERIAL_H
#include <QThread>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <strings.h>
#include <sys/select.h>


class serial:public QThread
{
    Q_OBJECT


public:
    serial();

    int serial_init(int fd);
    unsigned char getBCC(unsigned char *buf, int n);
    int serial_mian(char * serial_drive);
    int send_A_order(int fd);
    int send_B_older(int fd);

    void run(); //重写

signals:
    void returnCad( unsigned int cardid);

private:
    int serial_fd;
    char buf[100] = "/dev/ttySAC1";
};

#endif // SERIAL_H
