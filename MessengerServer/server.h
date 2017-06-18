#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "clientthread.h"
#include <QImage>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject* parent = nullptr);
    ~Server();

public slots:
    void start(qint16 port);
    void pause();
    void stop();

    void onMessageReceived(QMap<QString, QVariant> message);
    void onClientDisconnected(qintptr socketDescriptor);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    QMap<qintptr, ClientThread*> m_Clients;
    QSqlDatabase m_Database;
};

#endif // SERVER_H
