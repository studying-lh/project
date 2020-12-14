#include "keybetton.h"
#include <QDebug>
keyBetton::keyBetton(QWidget *parent) : QPushButton(parent)
{
    //在定义的按钮类中关联信号与槽
    connect(this,&keyBetton::clicked,this,&keyBetton::fun);
}

void keyBetton::fun()
{
    QKeyEvent *keyevent;
    keyBetton *bt=(keyBetton *)(sender());
    //获取按键的字面值
    QString str=bt->text();

    //判断特殊的按键
    if(str=="Enter") //回车
    {
        keyevent=new QKeyEvent(QEvent::KeyPress,Qt::Key_Enter,Qt::NoModifier,str);

    }
    else if(str=="back") //退格
    {
        keyevent=new QKeyEvent(QEvent::KeyPress,Qt::Key_Backspace,Qt::NoModifier,str);
    }
    else if(str=="space") //空格
    {
        keyevent=new QKeyEvent(QEvent::KeyPress,Qt::Key_Space,Qt::NoModifier," ");
    }
    else
    {
        //自定义按键事件
        keyevent=new QKeyEvent(QEvent::KeyPress,str.toInt(),Qt::NoModifier,str);
    }


    //发这个事件发送出去
    return QCoreApplication::postEvent(QApplication::focusWidget(),keyevent);
}

