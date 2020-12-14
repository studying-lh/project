#include "managemain.h"
#include "ui_managemain.h"
#include <QDebug>

ManageMain::ManageMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManageMain)
{
    ui->setupUi(this);
    /*列表设置*/
    /*设置列表的列数*/
    this->ui->show->setColumnCount(4);


    /*设置不显示格子线*/
    this->ui->show->setFrameShape(QFrame::NoFrame);

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
        qDebug()<<"manage open sql file failer";
    }else
    {
        qDebug()<<"manage open sql file ok";
    }

    /*对数据库进行操作*/
    this->query = new QSqlQuery(sql);

    /*判断表格是否存在*/
    if(!this->query->exec("create table if not exists tab(cad unsigned int unqiue,name text, info text,carNum int);"))
    {
        /*创建数据库表*/
        this->query->exec("create table if not  tab(cad unsigned int unqiue,name text, info text,carNum int);");
    }

    /*进行查询*/
    this->findSqlDate();

}

/*析构函数*/
ManageMain::~ManageMain()
{
    delete ui;
}

/*更新数据库数据*/
void ManageMain::findSqlDate()
{
    /*将列表清空*/
    this->ui->show->clear();

    /*表头设置*/
    QStringList headList;
    headList<<tr("卡号")<<tr("姓名")<<tr("用户信息")<<"车位";
    this->ui->show->setHorizontalHeaderLabels(headList);

    /*行数设为0*/
    this->ui->show->setRowCount(0);

    /*查询数据库表格*/
    if(!this->query->exec("select * from tab ;"))
    {
        qDebug()<<"find sql failer:"<<this->query->lastError().text();
    }

    /*填入表格*/
    for (int i = 0 ;this->query->next();i++)
    {
        /*获取当前的行数*/
        int rowIndex = this->ui->show->rowCount();

        /*添加行数*/
        this->ui->show->setRowCount(rowIndex+1);
        /*填入数据*/
        this->ui->show->setItem(i,0,new QTableWidgetItem(this->query->value(0).toString()));

        this->ui->show->setItem(i,1,new QTableWidgetItem(this->query->value(1).toString()));

        this->ui->show->setItem(i,2,new QTableWidgetItem(this->query->value(2).toString()));

        this->ui->show->setItem(i,3,new QTableWidgetItem(this->query->value(3).toString()));

    }


}




/*退出按钮*/
void ManageMain::on_retBt_clicked()
{
    /*发送信号*/
    emit this->RetSignals();
}

/*入户办理*/
void ManageMain::on_joinBT_clicked()
{
    /*入户界面创建*/
    this->join = new JoinList;

    /*管理员主界面隐藏*/
    this->hide();

    /*连接入户返回信号*/
    connect(this->join,&JoinList::RstSignal,this,&ManageMain::JoinRstThis);

    /*连接使用信号*/
    connect(this->join,&JoinList::UseSerialSignal,this,&ManageMain::JoinUserFun);

    /*入户界面显示*/
    this->join->show();


}

/*入户办理返回*/
void ManageMain::JoinRstThis()
{
    /*入户界面关闭*/
    this->join->close();

    /*入户界面使用RFID结束发送信号*/
    emit this->JoinUseEndSignal();

    /*进行查询*/
    this->findSqlDate();
    /*主界面显示*/
    this->show();

}


/*用户界面回应*/
void ManageMain::JoinRespond(serial *temp)
{
    this->join->MainWidgetRespond(temp);
}
/*入户需要使用的串口信号*/
void ManageMain::JoinUserFun()
{
     emit this->JoinUserSerial();
}


/*单击选中*/
void ManageMain::on_show_itemClicked(QTableWidgetItem *item)
{
    this->currentClick = item;

}
/*销户处理*/
void ManageMain::on_delBT_clicked()
{
    /*防止未点中 出现段错误*/
    if(this->currentClick == NULL)
    {
        return;
    }


    /*查询数据库表格*/
    if(!this->query->exec("select * from tab ;"))
    {
        qDebug()<<"find sql failer:"<<this->query->lastError().text();
    }

    /*填入表格*/
    while(this->query->next())
    {
        for (int i =0 ; i<4 ;i++)
        {
           if(this->query->value(i).toString() == this->currentClick->text())
           {
                /*进行不同表格的字符串拼接*/
                QString str;
                if(i ==0)
                {
                    str = QString("delete  from  tab  where cad=%1;")
                                    .arg(this->currentClick->text());
                }
                else if(i == 1)
                {
                    str = QString("delete  from  tab  where name=\"%1\";")
                                    .arg(this->currentClick->text());
                }
                else if(i == 2)
                {
                    str = QString("delete  from  tab  where info=\"%1\";")
                            .arg(this->currentClick->text());
                }
                else
                {
                    str = QString("delete  from  tabs  where carNum=%1;")
                            .arg(this->currentClick->text());
                }

                /*进行删除*/
              if(!this->query->exec(str))
              {
                  qDebug()<<"sql delete failer";
              }
              else
              {
                  /*删除成功则更新列表*/
                  this->findSqlDate();

                  /*初始化列表指针*/
                  this->currentClick  = NULL;
                  return;
              }
           }
        }
    }


}



