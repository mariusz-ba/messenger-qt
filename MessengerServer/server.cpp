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


        // TODO: fix sending image
        QImage image(":/images/img.png");
        QByteArray imageData;
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        QDataStream stream(&buffer);
        stream.setVersion(QDataStream::Qt_5_1);
        stream << image;
        buffer.close();

        qDebug() << "Image data: (" << imageData.size() << "): " << imageData;

        query.prepare("UPDATE users SET image=:image WHERE username='mariusz'");
        query.bindValue(":image", imageData);
        query.exec();


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
void Server::onMessageReceived(QMap<QString, QVariant> message)
{
    ClientThread* thread = qobject_cast<ClientThread*>(sender());
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

        //Checking database for user information
        QSqlQuery query(m_Database);
        query.prepare("SELECT username, password, name, surname, image FROM users WHERE username=:username");
        query.bindValue(":username", message["username"]);
        query.exec();

        if(query.last())
        {
            // User exists
            qDebug() << "User exists";
            QString username = query.value(0).toString();
            QString password = query.value(1).toString();

            qDebug() << "Password: " << message["password"] << " | Expected: " << password;

            // Check if password is correct
            if(password == message["password"])
            {
                // Password correct
                qDebug() << "Correct password";
                // Set all user data
                thread->client.username = message["username"].toString();
                thread->client.name = query.value(2).toString();
                thread->client.surname = query.value(3).toString();
                thread->client.image.loadFromData(query.value(4).toByteArray());
                thread->client.loggedin = true;

                qDebug() << thread->client.username << '\n' << thread->client.name << '\n' << thread->client.surname << '\n' << (thread->client.loggedin ? "Logged in" : "Not logged in");

                // Send user information to client
                QMap<QString, QVariant> response;
                response["command"] = "login";
                response["status"] = "success";
                response["name"] = thread->client.name;
                response["surname"] = thread->client.surname;
                response["image"] = thread->client.image;

                thread->write(response);
            }
            else
            {
                // Wrong password
                qDebug() << "Wrong password";

                QMap<QString, QVariant> response;
                response["command"] = "login";
                response["status"] = "wrong_password";

                thread->write(response);
            }
        }
        else
        {
            // User does not exist in database, register new one
            qDebug() << "User does not exist";

            QMap<QString, QVariant> response;
            response["command"] = "login";
            response["status"] = "wrong_username";

            thread->write(response);
        }



    }
    else if(message["command"] == "register")
    {
        // Register new user if name available
        QSqlQuery query(m_Database);
        query.prepare("SELECT COUNT(*) FROM users WHERE username=:username");
        query.bindValue(":username", message["username"]);
        query.exec();

        if(query.value(0) == 0)
        {
            // User does not exist, new one can be added
            QMap<QString, QVariant> response;
            response["command"] = "register";
            response["status"] = "success";
            thread->write(response);
        }
        else
        {
            // User already exists
            QMap<QString, QVariant> response;
            response["command"] = "register";
            response["status"] = "failure";
            thread->write(response);
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
            //if(iter.value()->client.username == message["to"])
            //{
                // Client is online, send message
                iter.value()->write(message);
            //}
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
    connect(thread, SIGNAL(messageReceived(QMap<QString,QVariant>)), this, SLOT(onMessageReceived(QMap<QString,QVariant>)));

    thread->start();

    m_Clients[socketDescriptor] = thread;
}

void Server::onClientDisconnected(qintptr socketDescriptor)
{
    // We just need to remove thread from map. Thread itself does all cleaning.
    m_Clients.remove(socketDescriptor);

    qDebug() << "Clients: " << m_Clients.size();
}
