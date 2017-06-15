#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include "clientthread.h"

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

    void onClientDisconnected(qintptr socketDescriptor);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    QMap<qintptr, ClientThread*> m_Clients;
};

#endif // SERVER_H
