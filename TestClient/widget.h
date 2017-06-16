#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QBuffer>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void sendMessage(const QMap<QString, QString>& message);

private slots:
    void on_buttonLogin_clicked();
    void on_buttonSend_clicked();

    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadyRead();

private:
    Ui::Widget *ui;
    QTcpSocket* m_Socket;
    QString m_Username;
};

#endif // WIDGET_H
