#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "server.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:

    void startServer();

private:
    Ui::Widget *ui;

    Server* m_Server;
};

#endif // WIDGET_H
