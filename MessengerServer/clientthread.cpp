#include "clientthread.h"
#include <QDebug>

ClientThread::ClientThread(qintptr socketDescriptor, QObject* parent)
    : QThread(parent), m_SocketDescriptor(socketDescriptor)
{
}

void ClientThread::run()
{
    qDebug() << "Client thread started for id: " << m_SocketDescriptor;

    m_Socket = new QTcpSocket();

    if(!m_Socket->setSocketDescriptor(m_SocketDescriptor))
    {
        // Something wrong happened lol
        emit error(m_Socket->error());
        return;
    }

    connect(m_Socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(m_Socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

    // Make this thread a loop, so that signal/slot to function properly not
    // dropped out in the middle when thread dies
    exec();
}

// Send serialized message to client
void ClientThread::write(QMap<QString, QString> message)
{
    // Creating and openning buffer
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);

    // Using stream to serialize data in buffer
    QDataStream stream(&buffer);
    stream.setVersion(QDataStream::Qt_5_1);
    stream << message;

    // Get all serialized data from buffer
    QByteArray Data = buffer.readAll();

    // Send data to client
    m_Socket->write(Data);
    m_Socket->waitForBytesWritten(3000);
    m_Socket->flush();

    // Closing buffer
    buffer.close();
}

void ClientThread::onReadyRead()
{
    QByteArray Data = m_Socket->readAll();

    qDebug() << m_SocketDescriptor << " Data in: " << Data;

    QBuffer buffer(&Data);
    buffer.open(QIODevice::ReadOnly);

    QDataStream stream(&buffer);
    stream.setVersion(QDataStream::Qt_5_1);

    QMap<QString, QString> message;
    stream >> message;

    buffer.close();

    // Pass recived data to server
    emit messageReceived(message);
}

void ClientThread::onDisconnected()
{
    qDebug() << m_SocketDescriptor << " Disconnected";

    // emit disconnect signal so the server will be able to properly remove thread from list
    emit clientDisconnected(m_SocketDescriptor);

    m_Socket->deleteLater();
    exit(0);
}
