#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
/*时间*/
#include <QTime>
#include <QDateTime>
#include <QTimer>
#include <QPixmap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QSqlError>
#include <QVariant>
#include <QSqlRecord>
#include <QList>
#include <QProcess>

#include "serial.h"
#include "video.h"
#include "managewidget.h"
#include "httpget.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void TimeShow();
    void videoShow(struct videoBufInfo temp);
    void serverCad(unsigned int date);
    QString  getCurrentTime();
    void ManageIoginRet();
    void JoinUserFun();
    void JoinUseEndFun();
    int ListRecord(unsigned int a);
    void protectUser();
    void SetTime(QString str);
    void getWeather(QString str);
signals:
    void GetCardWin(QString str);
    void JoinUserSignal();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer currentShowTimer;
    QTime currentTime;
    /*创建RDIF对象*/
    serial serialObject;

    Video * video;
    ManageWidget * manage;

    QSqlQuery * query;

    QSqlDatabase sql;
    QList<int> list;
    QTimer protectTimer;
    httpGet * http;
};
#endif // MAINWINDOW_H
