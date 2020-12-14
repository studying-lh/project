#include "managewidget.h"
#include "ui_manage.h"

#include <QDebug>
ManageWidget::ManageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Manage)
{
    /*ui界面设置*/
    this->ui->setupUi(this);

    /*密码隐藏*/
    this->ui->passwdLine->setEchoMode(QLineEdit::Password);

    /*设置管理员账号*/
    this->ManageUser.clear();
    this->ManageUser.append("gec");

    /*设置管理员密码*/
    this->ManagePasswd.clear();
    this->ManagePasswd.append("123");

    /*错误提醒*/
    this->ui->errorLabe->hide();
}

/*析构函数*/
ManageWidget::~ManageWidget()
{
    delete  ui;
}

/*重写事件*/
bool ManageWidget::eventFilter(QObject *watched, QEvent *event)
{

    if(event->type()==QEvent::Enter)
    {
        qDebug()<<"enter";
    }
    else if(event->type()==QEvent::Leave)
    {
        qDebug()<<"Leave";
    }

    return ManageWidget::eventFilter(watched,event);
}



/*确认按钮*/
void ManageWidget::on_sureBt_clicked()
{
    /*获取账号*/
    QString user = this->ui->UserLine->text();

    /*获取密码*/
    QString passwd = this->ui->passwdLine->text();

    /*判断账号和密码*/
    if(user == this->ManageUser &&passwd == this->ManagePasswd)
    {
        /*隐藏本界面*/
        this->hide();
        /*管理员主界面*/
        this->manageMain = new ManageMain;

        /*连接入户信号*/
        connect(this->manageMain,&ManageMain::JoinUserSerial,this,&ManageWidget::JoinUserFun);

        /*连接入户使用完的信号*/
        connect(this->manageMain,&ManageMain::JoinUseEndSignal,this,&ManageWidget::JoinUerEenFun);

        /*显示管理员主界面*/
        this->manageMain->show();

        /*连接返回信号*/
        connect(this->manageMain,&ManageMain::RetSignals,this,&ManageWidget::ManageMainRet);
    }
    /*输入错误*/
    else
    {
        /*设置错误提示*/
        this->ui->errorLabe->setText("输入错误");
        /*显示标签*/
        this->ui->errorLabe->show();
        /*清空输入栏*/
        this->ui->UserLine->clear();
        this->ui->passwdLine->clear();
    }

}

/*管理员主界面的返回信号*/
void ManageWidget::ManageMainRet()
{
    /*调用直接返回*/
    on_retBt_clicked();
}



/*入户信号处理*/
void ManageWidget::JoinUserFun()
{
    emit this->JoinUserSignalTemp();
}

/*用户界面回应*/
void ManageWidget::JoinRespond(serial *temp)
{
    this->manageMain->JoinRespond(temp);
}

/*入户结束信号*/
void ManageWidget::JoinUerEenFun()
{
    emit this->JoinUserEndSignalTemp();
}


/*返回按钮*/
void ManageWidget::on_retBt_clicked()
{
    /*清空输入栏*/
    this->ui->UserLine->clear();
    this->ui->passwdLine->clear();

    /*发送返回信号*/
    emit this->RetSignals();
}
