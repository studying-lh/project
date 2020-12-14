#ifndef KEY_H
#define KEY_H

#include <QWidget>

namespace Ui {
class key;
}

class key : public QWidget
{
    Q_OBJECT

public:
    explicit key(QWidget *parent = nullptr);
    ~key();

private:
    Ui::key *ui;
};

#endif // KEY_H
