#ifndef MANAGEMAIN_H
#define MANAGEMAIN_H

#include <QWidget>
#include "joinlist.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QSqlError>
#include <QVariant>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include "serial.h"

namespace Ui {
class ManageMain;
}

class ManageMain : public QWidget
{
    Q_OBJECT

public:
    explicit ManageMain(QWidget *parent = nullptr);
    ~ManageMain();

    void findSqlDate();
    void JoinRstThis();

    void JoinRespond(serial * temp);
    void JoinUserFun();
signals:
    void RetSignals();
    void JoinUserSerial();
    void JoinUseEndSignal();
private slots:
    void on_retBt_clicked();

    void on_joinBT_clicked();

    void on_show_itemClicked(QTableWidgetItem *item);

    void on_delBT_clicked();



private:
    Ui::ManageMain *ui;
    JoinList *join;
    QSqlDatabase sql;
    QSqlQuery * query;
    QTableWidgetItem * currentClick;
};

#endif // MANAGEMAIN_H
