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

void ClientThread::onReadyRead()
{
    QByteArray Data = m_Socket->readAll();

    // Pass recived data to server

    qDebug() << m_SocketDescriptor << " Data in: " << Data;

    m_Socket->write(Data);
    m_Socket->waitForBytesWritten(3000);
    m_Socket->flush();
}

void ClientThread::onDisconnected()
{
    qDebug() << m_SocketDescriptor << " Disconnected";

    // emit disconnect signal so the server will be able to properly remove thread from list
    emit clientDisconnected(m_SocketDescriptor);

    m_Socket->deleteLater();
    exit(0);
}
