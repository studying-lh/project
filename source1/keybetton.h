#ifndef KEYBETTON_H
#define KEYBETTON_H

#include <QPushButton>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QApplication>


class keyBetton : public QPushButton
{

public:
    explicit keyBetton(QWidget *parent = 0);


public slots:
    void fun();
};

#endif // KEYBETTON_H
