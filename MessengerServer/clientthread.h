#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <QThread>
#include <QTcpSocket>

#include <QDataStream>
#include <QBuffer>

#include <QImage>

struct Client
{
    QString username; // Unique username
    QString name;
    QString surname;
    QImage image;
    bool loggedin;
};

class ClientThread : public QThread
{
    Q_OBJECT
public:
    explicit ClientThread(qintptr socketDescriptor, QObject* parent = nullptr);
    void run() override;

    Client client;

    void write(QMap<QString, QVariant> message);

signals:
    void messageReceived(QMap<QString, QVariant> message);
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
