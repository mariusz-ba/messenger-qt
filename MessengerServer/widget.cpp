#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    m_Server = new Server(this);

    connect(ui->buttonStart, SIGNAL(clicked(bool)), this, SLOT(startServer()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::startServer()
{
    m_Server->start(ui->lineEdit->text().toInt());
}
