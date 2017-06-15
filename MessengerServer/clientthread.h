#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <QThread>
#include <QTcpSocket>

class ClientThread : public QThread
{
    Q_OBJECT
public:
    explicit ClientThread(qintptr socketDescriptor, QObject* parent = nullptr);
    void run() override;

signals:
    void clientDisconnected(qintptr socketDescriptor);
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket* m_Socket;
    qintptr m_SocketDescriptor;
};

#endif // CLIENTTHREAD_H
