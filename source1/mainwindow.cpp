#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*设置窗口大小*/
    this->setFixedSize(800,480);
    /*获取年份*/
    QDateTime currentYMD = QDateTime::currentDateTime();
    QString yearStr = currentYMD.toString("yyyy");
    QString monthStr =currentYMD.toString("MM");
    QString dayStr = currentYMD.toString("dd");

    /*启动http请求*/
    if(yearStr.toInt() <= 2020)
    {
        /*创建时间获取对象*/
        this->http = new httpGet;

        /*连接设置时间*/
        connect(this->http,&httpGet::getHttpTime,this,&MainWindow::SetTime);
    }
    else
    {
        /*年份拼接*/
        QString YMDShow = QString("%1年%2月%3日")
                                   .arg(yearStr).arg(monthStr).arg(dayStr);
        /*年份显示*/
        this->ui->YMDtime->setText(YMDShow);

        /*当前系统时间获取*/
        this->currentTime = QTime::currentTime();

        /*设置LCD的个数*/
        this->ui->lcdNumber->setDigitCount(8);
        /*设置LCD的数值*/
        this->ui->lcdNumber->display(this->currentTime.toString("hh:mm:ss"));

        /*秒数刷新*/
        connect(&this->currentShowTimer,&QTimer::timeout,this,&MainWindow::TimeShow);
        this->currentShowTimer.start(1000);
    }


    /*隐藏文本*/
    this->ui->nameshow->hide();
    this->ui->userInfor->hide();


    /*启动RDIF*/
    this->serialObject.start();
    connect(&this->serialObject,&serial::returnCad,this,&MainWindow::serverCad);

    /*摄像头对象*/
    this->video = new Video;

    /*摄像头启动*/
    this->video->start();

    /*管理员界面初始化*/
    this->manage = new ManageWidget;

    /*添加数据库驱动*/
    this->sql = QSqlDatabase::addDatabase("QSQLITE");

    /*添加数据库名字*/
    sql.setDatabaseName("frist.db");

    /*打开数据库文件*/
    if(!sql.open())
    {
        qDebug()<<"mian sql open file failer";
    }else
    {
        qDebug()<<"mian sql open file ok";
    }

    /*对数据库进行操作*/
    this->query = new QSqlQuery(sql);

    /*判断表格是否存在*/
    if(!this->query->exec("create table if not exists tab(cad long ,name text, info text,carNum int);"))
    {
        /*创建数据库表*/
        if(!this->query->exec("create table tab(cad long ,name text, info text,carNum int);"))
        {
            qDebug()<<"create sql table failer";
        }
        /*创建成功提示*/
        else
        {
            qDebug()<<"create sql table ok";
        }
    }
    /*表格存在提示*/
    else
    {
        qDebug()<<"sql table exists";
    }

    /*链表容器初始化*/
    this->list.clear();

}

MainWindow::~MainWindow()
{
    /*关闭RDIF*/
    this->serialObject.terminate();
    /*关闭摄像头*/
    this->video->terminate();

    delete ui;
}

/*时间显示*/
void MainWindow::TimeShow()
{
    this->currentTime = QTime::currentTime();
    /*设置LCD的数值*/
    this->ui->lcdNumber->display(this->currentTime.toString("hh:mm:ss"));

    /*日期更新*/
    if(this->currentTime.toString("hh:mm:ss")=="00:00:00")
    {
        /*获取系统年月日*/
        QString YMDShow = this->getCurrentTime();
        this->ui->YMDtime->setText(YMDShow);
    }

}




/*接受卡号*/
void MainWindow::serverCad(unsigned int date)
{

    qDebug()<<"mian widget:"<<date;
    this->protectTimer.stop();

    /*对数据库进行查询*/
    if(!this->query->exec("select * from tab "))
    {
        qDebug()<<"find sqliet failer："<<this->query->lastError().text();

        return ;
    }
    /*设置标志位*/
    int flag = 0;
    /*循环查询*/
    while(this->query->next())
    {
        /*卡号符合*/
        if(this->query->value(0).toUInt() == date)
        {
            /*进行链表操作*/
           int ret = this->ListRecord(this->query->value(0).toUInt());
            /*显示信息*/

            /*姓名文本设置*/
            this->ui->nameshow->setText(this->query->value(1).toString());
            /*姓名显示*/
            this->ui->nameshow->show();
            /*信息文本设置*/
            QString inforStr;
            if(ret == 0)
            {
                inforStr = QString("%1用户 欢迎回家").arg(this->query->value(2).toString());
            }
            else
            {
                inforStr = QString("%1用户 一路顺风").arg(this->query->value(2).toString());
            }
            this->ui->userInfor->setText(inforStr);
            /*信息显示*/
            this->ui->userInfor->show();
            /*车位显示*/
            QString carStr =QString("剩余停车位：%1").arg(this->query->value(3).toString());
            this->ui->carNum->setText(carStr);

            /*使标志位为1 表示为小区的住户*/
            flag = 1;
            break;
        }
    }

    /*判断标志位*/
    if(flag != 1)
    {
        /*设置姓名*/
        QString tempName =  QString("卡号%1").arg(date);
        this->ui->nameshow->setText(tempName);
        /*显示*/
        this->ui->nameshow->show();

        /*设置提醒语*/
        this->ui->userInfor->setText("非小区住户或未注册");
        /*显示*/
        this->ui->userInfor->show();
    }

    /*设置保护信息定时*/
    connect(&this->protectTimer,&QTimer::timeout,this,&MainWindow::protectUser);
    this->protectTimer.start(9000);

    /*刷卡成功进行拍照*/
    /*获取年份*/
    QDateTime currentYMD = QDateTime::currentDateTime();
    QString yearStr = currentYMD.toString("yyyy");
    QString monthStr =currentYMD.toString("MM");
    QString dayStr = currentYMD.toString("dd");
    QString weekStr = currentYMD.toString("ddd");

    QString ymd = QString("%1.%2.%3")
                               .arg(yearStr).arg(monthStr).arg(dayStr);
    QString Cadtime = this->currentTime.toString("hh.mm.ss");
    QString CadStr = QString("%1_%2_%3.jpg").arg(date).arg(ymd).arg(Cadtime);

    this->video->MianCaller(CadStr);


}

/*获取系统年份*/
QString MainWindow::getCurrentTime()
{
    /*获取年份*/
    QDateTime currentYMD = QDateTime::currentDateTime();
    QString yearStr = currentYMD.toString("yyyy");
    QString monthStr =currentYMD.toString("MM");
    QString dayStr = currentYMD.toString("dd");
    QString weekStr = currentYMD.toString("ddd");

    QString YMDShow = QString("%1年%2月%3日")
                               .arg(yearStr).arg(monthStr).arg(dayStr);
    return YMDShow;
}




/*管理员登陆*/
void MainWindow::on_pushButton_clicked()
{
    /*隐藏本界面*/
    this->hide();

    /*连接入户信号*/
    connect(this->manage,&ManageWidget::JoinUserSignalTemp,this,&MainWindow::JoinUserFun);

    /*显示管理员界面*/
    this->manage->show();

    /*返回信号关联*/
    connect(this->manage,&ManageWidget::RetSignals,this,&MainWindow::ManageIoginRet);
}

/*管理员登陆界面返回*/
void MainWindow::ManageIoginRet()
{
    /*登陆界面隐藏*/
    this->manage->hide();

    /*主界面显示*/
    this->show();
}

/*用户使用串口信号*/
void MainWindow::JoinUserFun()
{
    /*断开连接*/
    disconnect(&this->serialObject,&serial::returnCad,this,&MainWindow::serverCad);
    /*传递参数*/
    this->manage->JoinRespond(&this->serialObject);
    /*连接用户使用完毕的信号*/
    connect(this->manage,&ManageWidget::JoinUserEndSignalTemp,this,&MainWindow::JoinUseEndFun);
}

/*用户使用完的信号处理函数*/
void MainWindow::JoinUseEndFun()
{
    /*重新连接*/
    connect(&this->serialObject,&serial::returnCad,this,&MainWindow::serverCad);
}

/*链表操作*/
int  MainWindow::ListRecord(unsigned int a)
{
    /*寻找和链表的匹配的数据*/
    for (int i = 0; i < this->list.size();i++)
    {
        if(this->list.at(i) == (int)a)
        {
            /*删除数据*/
            this->list.takeAt(i);
            /*返回1 表示出门*/
            return 1;
        }
    }

    /*匹配不到 表示进入 添加数据进链表*/
    this->list.append(a);
    /*返回0 表示进门*/
    return 0;

}

/*保护信息定时*/
void MainWindow::protectUser()
{
    /*停止定时器*/
    this->protectTimer.stop();

    /*信息隐藏*/
    this->ui->userInfor->hide();
    this->ui->nameshow->hide();

    this->ui->carNum->setText("剩余停车位：");

}
/*设置本地时间*/
void MainWindow::SetTime(QString str)
{
    /*设置时间命令*/
    QString date1 = QString("date -s \"%1\"").arg(str);

    QProcess::startDetached(date1);
    /*同步系统时间*/
    QProcess::startDetached("hwclock -w");
    /*保存配置*/
    QProcess::startDetached("sync");

    /*获取年份*/
    QDateTime currentYMD = QDateTime::currentDateTime();
    QString yearStr = currentYMD.toString("yyyy");
    QString monthStr =currentYMD.toString("MM");
    QString dayStr = currentYMD.toString("dd");

    /*年份拼接*/
    QString YMDShow = QString("%1年%2月%3日")
                               .arg(yearStr).arg(monthStr).arg(dayStr);
    /*年份显示*/
    this->ui->YMDtime->setText(YMDShow);

    /*当前系统时间获取*/
    this->currentTime = QTime::currentTime();

    /*设置LCD的个数*/
    this->ui->lcdNumber->setDigitCount(8);
    /*设置LCD的数值*/
    this->ui->lcdNumber->display(this->currentTime.toString("hh:mm:ss"));

    /*秒数刷新*/
    connect(&this->currentShowTimer,&QTimer::timeout,this,&MainWindow::TimeShow);
    this->currentShowTimer.start(1000);

    /*获取天气*/
    connect(this->http,&httpGet::getWeatherStr,this,&MainWindow::getWeather);
    QString weatherStr = QString("%1-%2-%3").arg(yearStr).arg(monthStr).arg(dayStr);
    this->http->GetWeather(weatherStr);

}

/*获取天气字符串*/
void MainWindow::getWeather(QString str)
{
    /*设置天气标签文本*/
    this->ui->weatherLab->setText(str);
}
