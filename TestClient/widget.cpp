#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    m_Socket = new QTcpSocket(this);
    connect(m_Socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(m_Socket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
    connect(m_Socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::sendMessage(const QMap<QString, QVariant> &message)
{
    //if(m_Socket->state() != QTcpSocket::ConnectedState)
        //return;

    qDebug() << "Sending data";
    // Serialize message map
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QDataStream stream(&buffer);
    stream.setVersion(QDataStream::Qt_5_1);
    stream << message;

    // Get data from buffer
    buffer.seek(0);
    QByteArray data = buffer.readAll();
    qDebug() << data;

    // Send data
    m_Socket->write(data);
    m_Socket->waitForBytesWritten(3000);
    m_Socket->flush();

    // Close buffer
    buffer.close();
}

void Widget::on_buttonLogin_clicked()
{
    m_Username = ui->lineEditLogin->text();

    m_Socket->connectToHost("127.0.0.1", 1234);
}

void Widget::on_buttonSend_clicked()
{
    // Create message map
    QMap<QString, QVariant> message;
    message["command"] = "send";
    message["from"] = m_Username;
    message["to"] = "client2";
    message["message"] = ui->lineEditMessage->text();

    // Send map
    sendMessage(message);
}

void Widget::onSocketConnected()
{
    ui->lineEditStatus->setText("Connected to server");
    qDebug() << "Connected to server";

    // When user has connected to the server try to log in
    QMap<QString, QVariant> message;
    message["command"] = "login";
    message["username"] = m_Username;
    message["password"] = ui->lineEditPassword->text();

    // Send map
    sendMessage(message);
}

void Widget::onSocketDisconnected()
{
    ui->lineEditStatus->setText("Disconnected");
    qDebug() << "Disconnected";
}

void Widget::onSocketReadyRead()
{
    QMap<QString, QVariant> message;

    QByteArray data = m_Socket->readAll();
    // data is represented by map
    qDebug() << "Data received: " << data;
    // Serialize message map
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadWrite);
    QDataStream stream(&buffer);
    stream.setVersion(QDataStream::Qt_5_1);
    stream >> message;

    qDebug() << message.size();

    // Get data from buffer
    if(message["command"] == "send")
    {
        ui->textBrowser->append(message["from"].toString() + ": " + message["message"].toString());
        qDebug() << "Nadawca: " << message["from"];
        qDebug() << "Wiadomosc: " << message["message"];
    }
    else if(message["command"] == "login")
    {
        qDebug() << "Login: " << message["status"];
        if(message["status"] == "success")
        {
            qDebug() << "Your name: " << message["name"].toString() + " " + message["surname"].toString();
            m_UserImage = message["image"].value<QImage>();
            m_Pixmap.fromImage(m_UserImage);
            ui->label->setPixmap(m_Pixmap);
        }
    }
    else if(message["command"] == "register")
    {
        qDebug() << "Register: " << message["status"];
    }

    // Close buffer
    buffer.close();
}
