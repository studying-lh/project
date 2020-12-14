#ifndef JOINLIST_H
#define JOINLIST_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QSqlError>
#include <QVariant>
#include <QSqlRecord>
#include <QTimer>
#include "serial.h"

namespace Ui {
class JoinListwidget;
}

class JoinList : public QWidget
{
    Q_OBJECT

public:
    explicit JoinList(QWidget *parent = nullptr);
    ~JoinList();
    void MainWidgetRespond(serial * temp);

    int  findCadOnly(QString  str);
    void SendUserSignal();
    void CarGet(unsigned int a);
private slots:
    void on_addBT_clicked();

    void on_RstBt_clicked();

signals:
    void RstSignal();
    void UseSerialSignal();
private:
    Ui::JoinListwidget  *ui;

    QSqlQuery * query;
    QSqlDatabase sql;
    /*创建RDIF对象指针*/
    serial* serialObject;
    QTimer timer;

};

#endif // JOINLIST_H
