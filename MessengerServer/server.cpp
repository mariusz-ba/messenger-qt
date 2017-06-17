#include "server.h"
#include <QDebug>

Server::Server(QObject* parent)
    : QTcpServer(parent)
{
    m_Database = QSqlDatabase::addDatabase("QSQLITE");

    m_Database.setDatabaseName("../../messenger.db");
    if(!m_Database.open())
    {
        qDebug() << "Could not open database";
    }
    else
    {
        qDebug() << "Connected to database";

        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, username TEXT UNIQUE, password TEXT, name TEXT, surname TEXT, image BLOB)");
        query.exec("CREATE TABLE IF NOT EXISTS messages (id_from INTEGER, id_to INTEGER, message TEXT, datetime TEXT)");

        qDebug() << "Tables: " << m_Database.tables().join(',');
    }
}

Server::~Server()
{
    for(auto iter = m_Clients.begin(); iter != m_Clients.end(); ++iter)
    {
        iter.value()->onDisconnected();
    }

    m_Database.close();
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
    qDebug() << "Size of passed message is: " << message.size();
    if(message["command"] == "login")
    {
        qDebug() << message["username"] << " is trying to log in.";
        // User tried to log in

        //TODO: check database for user
        QSqlQuery query(m_Database);
        query.prepare("SELECT username, password, name, surname, image FROM users WHERE username=:username");
        query.bindValue(":username", message["username"]);
        query.exec();

        //qDebug() << "Rows returned: " << query.last();

        if(query.last())
        {
            qDebug() << "User exists";
            // User exists
            QString username = query.value(0).toString();
            QString password = query.value(1).toString();

            qDebug() << "Password: " << message["password"] << " | Expected: " << password;

            // Check if password is correct
            if(password == message["password"])
            {
                // Password correct
                qDebug() << "Correct password";
                // Set all user data
                ClientThread* thread = qobject_cast<ClientThread*>(sender());
                thread->client.username = message["username"];
                thread->client.name = query.value(2).toString();
                thread->client.surname = query.value(3).toString();
                thread->client.image.loadFromData(query.value(4).toByteArray());
                thread->client.loggedin = true;

                qDebug() << thread->client.username << '\n' << thread->client.name << '\n' << thread->client.surname << '\n' << (thread->client.loggedin ? "Logged in" : "Not logged in");
            }
            else
            {
                // Wrong password
                qDebug() << "Wrong password";
            }
        }
        else
        {
            // User does not exist in database, register new one
            qDebug() << "User does not exist";
        }



    }
    else if(message["command"] == "send")
    {
        qDebug() << "Message received.";
        // Update message in database and send to client if online

        // TODO: Add message to database
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
    connect(thread, SIGNAL(messageReceived(QMap<QString,QString>)), this, SLOT(onMessageReceived(QMap<QString,QString>)));

    thread->start();

    m_Clients[socketDescriptor] = thread;
}

void Server::onClientDisconnected(qintptr socketDescriptor)
{
    // We just need to remove thread from map. Thread itself does all cleaning.
    m_Clients.remove(socketDescriptor);

    qDebug() << "Clients: " << m_Clients.size();
}
