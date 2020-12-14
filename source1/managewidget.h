#ifndef MANAGEWIDGET_H
#define MANAGEWIDGET_H

#include <QWidget>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QApplication>
#include "managemain.h"
#include "serial.h"
namespace Ui { class Manage; }

class ManageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ManageWidget(QWidget *parent = nullptr);
    ~ManageWidget();
    bool eventFilter(QObject *watched, QEvent *event);
    void ManageMainRet();

    void JoinUserFun();
    void JoinRespond(serial * temp);
    void JoinUerEenFun();
signals:
    void RetSignals();
    void JoinUserSignalTemp();
    void JoinUserEndSignalTemp();
private slots:
    void on_sureBt_clicked();

    void on_retBt_clicked();

private:
    Ui::Manage *ui;
    QString ManageUser;
    QString ManagePasswd;
    ManageMain *  manageMain;
    QSqlDatabase * tempSql;
};

#endif // MANAGEWIDGET_H
