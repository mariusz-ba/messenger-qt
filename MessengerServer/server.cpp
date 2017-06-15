#include "server.h"
#include <QDebug>

Server::Server(QObject* parent)
    : QTcpServer(parent)
{
}

Server::~Server()
{
    for(auto iter = m_Clients.begin(); iter != m_Clients.end(); ++iter)
    {
        iter.value()->onDisconnected();
    }
}

void Server::start(qint16 port)
{
    if(!this->listen(QHostAddress::Any, port))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listenning on: " << serverAddress().toString() << ":" << serverPort();
    }
}

void Server::pause()
{

}

void Server::stop()
{

}

// This function is called by QTcpServer when a new connection is available
void Server::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << socketDescriptor << " Connecting...";

    // Every new connection will be run in a newly created thread
    ClientThread* thread = new ClientThread(socketDescriptor, this);

    connect(thread, SIGNAL(clientDisconnected(qintptr)), this, SLOT(onClientDisconnected(qintptr)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();

    m_Clients[socketDescriptor] = thread;
}

void Server::onClientDisconnected(qintptr socketDescriptor)
{
    // We just need to remove thread from map. Thread itself does all cleaning.
    m_Clients.remove(socketDescriptor);

    qDebug() << "Clients: " << m_Clients.size();
}
