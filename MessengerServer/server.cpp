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

// This function is called when new message is received
void Server::onMessageReceived(QMap<QString, QString> message)
{
    /**
     *
     * Example map input
     * QMap["command"] = "send";
     * QMap["from"] = "username";
     * QMap["to"] = "username";
     * QMap["message"] = "content";
     *
     * QMap["command"] = "changeuserdata";
     * QMap["username"] = "username";
     * QMap["newpassword"] = "newpassword";
     *
     */
    if(message["command"] == "login")
    {
        // User tried to log in

        //TODO: check database for user

        ClientThread* thread = qobject_cast<ClientThread*>(sender());
        thread->client.username = message["username"];
    }
    else if(message["command"] == "send")
    {
        // Update message in database and send to client if online

        // Add message to database
        // ...

        // Send to client if online
        for(auto iter = m_Clients.begin(); iter != m_Clients.end(); ++iter)
        {
            if(iter.value()->client.username == message["to"])
            {
                // Client is online, send message
                iter.value()->write(message);
            }
        }

    }
    else if(message["command"] == "refresh")
    {
        // Get all messages for given user and send them. This command is sent every time user connects to the server
    }
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
