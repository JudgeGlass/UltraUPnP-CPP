#include "routerfinder.h"

#include <QDebug>

RouterFinder::RouterFinder(QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket(this);
    SSDP_IP_HOST = new QHostAddress(SSDP_IP);
    udpSocket->setSocketOption(QAbstractSocket::MulticastTtlOption, 2);
   // udpSocket->bind(1900, QHostAddress::LocalHost);

    connect(udpSocket, &QUdpSocket::readyRead, this, &RouterFinder::read);
}

void RouterFinder::read(){
    QByteArray routerResponse;
    routerResponse.resize(udpSocket->pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;
    udpSocket->readDatagram(routerResponse.data(), routerResponse.size(), &sender, &senderPort);
    qDebug("Router Response: \n");
    QString strRouterResponse(routerResponse);
    QRegExp rx("((?:https?|ftp)://\\S+)");
    int pos = rx.indexIn(strRouterResponse, 0);
    if (-1 != pos)
    {
        QString cap = rx.cap(0);
        cap = cap.left(cap.indexOf('\''));
        descriptorURL.append(cap);
    }
}

QString RouterFinder::getDescriptorURL() const{
    return descriptorURL;
}

void RouterFinder::search() const{
    qDebug("Sending multicast request...\n");
    QString requestMessage;
    requestMessage.append("M-SEARCH * HTTP/1.1\r\n");
    requestMessage.append("HOST: " + SSDP_IP + ":" + QString::number(SSDP_PORT) + "\r\n");
    requestMessage.append("ST: urn:schemas-upnp-org:device:InternetGatewayDevice:1\r\n");
    requestMessage.append("MAN: \"ssdp:discover\"\r\n");
    requestMessage.append("MX: 2\r\n");
    requestMessage.append("\n");

    QByteArray bArray = QByteArray::fromStdString(requestMessage.toStdString());
    udpSocket->writeDatagram(bArray.data(), bArray.size(), *SSDP_IP_HOST, 1900);
}

RouterFinder::~RouterFinder(){
    delete udpSocket;
    delete SSDP_IP_HOST;
}
