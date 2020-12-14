#include "key.h"
#include "ui_key.h"

key::key(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::key)
{
    ui->setupUi(this);
}

key::~key()
{
    delete ui;
}
