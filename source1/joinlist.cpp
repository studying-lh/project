#include "joinlist.h"
#include "ui_joinlist.h"
#include "QDebug"
JoinList::JoinList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JoinListwidget)
{
    ui->setupUi(this);


    /*打开数据库驱动*/
    if(QSqlDatabase::contains("qt_sql_default_connection"))
    {
        this->sql = QSqlDatabase::database("qt_sql_default_connection");
    }else
    {
        this->sql = QSqlDatabase::addDatabase("QSQLITE");
    }

    /*添加数据库名字*/
    sql.setDatabaseName("frist.db");

    /*打开数据库文件*/
   if(!sql.open())
   {
       qDebug()<<"sql file open failer";
   }else
   {
       qDebug()<<"sql file open ok";
   }

    /*对数据库进行操作*/
    this->query = new QSqlQuery(sql);

    /*查询住户*/
    this->query->exec("select * from tab;");

   /*下拉框添加*/
   for (int i = 0; i<20;i++)
   {
       /*添加信息*/
       QString str = QString("%1号").arg(i);

       /*标志位*/
       int flag = 0;

       /*确认已经注册*/
       while(this->query->next())
       {
           if(this->query->value(2).toString() == str)
           {
               flag = 1;
               break;
           }
       }

       /*判断标志位*/
       if(flag == 1)
       {
           continue;
       }

       /*添加下拉项*/
       this->ui->box->addItem(str);
   }

    /*提示文本隐藏*/
    this->ui->hintLab->hide();
    /*设置只读*/
    this->ui->cadLine->setReadOnly(true);

    /*设置定时*/
    connect(&this->timer,&QTimer::timeout,this,&JoinList::SendUserSignal);

    /*定时启动*/
    this->timer.start(3000);
}

JoinList::~JoinList()
{
    delete ui;
}

/*确认槽*/
void JoinList::on_addBT_clicked()
{


    /*获取名字*/
    QString nameStr = this->ui->UserLine->text();

    /*获取卡号*/
    QString cadStr = this->ui->cadLine->text();

    /*获取车位*/
    QString carNum = this->ui->carNumLine->text();

    /*获取住户信息*/
    QString userIofor = this->ui->box->currentText();

    /*是否被注册验证*/
    if(this->findCadOnly(cadStr)==1)
    {
        /*设置提示标签文本*/
        this->ui->hintLab->setText("此卡已被注册");
        this->ui->hintLab->show();
        return ;
    }

    /*存入数据库中*/


    /*插入数据*/
    QString userStr = QString("insert into tab values (%1,\"%2\",\"%3\",%4);")
                                .arg(cadStr.toInt()).arg(nameStr).arg(userIofor).arg(carNum.toInt());
    if(!this->query->exec(userStr))
    {
        /*设置提示标签文本*/
        this->ui->hintLab->setText("添加失败");
        this->ui->hintLab->show();
    }else
    {
        /*设置提示标签文本*/
        this->ui->hintLab->setText("添加成功");
        this->ui->hintLab->show();

        /*清空输入栏*/
        this->ui->cadLine->clear();
        this->ui->UserLine->clear();
        this->ui->carNumLine->clear();

        /*信息去除*/
        this->ui->box->removeItem(this->ui->box->findText(userIofor));
    }


}

/*返回槽*/
void JoinList::on_RstBt_clicked()
{
    /*发送返回信号*/
    emit this->RstSignal();
}



/*验证卡号的唯一性*/
int JoinList::findCadOnly(QString str)
{


    /*查询数据库表格*/
    if(!this->query->exec("select cad from tab"))
    {
        qDebug()<<"find sql failer:"<<this->query->lastError().text();
    }

    /*查询卡号*/
    while(this->query->next())
    {
        /*卡号比对*/
        if(this->query->value(0).toString() == str)
        {
            return 1;
        }
    }

    return 0;

}
/*发送信号*/
void JoinList::SendUserSignal()
{
    /*发送使用信号的请求*/
    emit this->UseSerialSignal();
}
/*主界面的回应*/
void JoinList::MainWidgetRespond(serial *temp)
{
    /*接收*/
    this->serialObject = temp;
    /*判断中途是否错误*/
    if(this->serialObject != NULL)
    {
        /*停止信号发送*/
        this->timer.stop();
        connect(this->serialObject,&serial::returnCad,this,&JoinList::CarGet);
    }

}

/*卡号获取*/
void JoinList::CarGet(unsigned int a)
{
    /*获取的卡号直接添加到输入栏上*/
    this->ui->cadLine->setText(QString::number(a));
}
