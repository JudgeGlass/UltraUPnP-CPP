#include "routerfinder.h"

#include <QDebug>

RouterFinder::RouterFinder(QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket(this);
    MY_IP = new QHostAddress("192.168.86.22");
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
    qDebug() << routerResponse;
}

QString RouterFinder::getUPnPUrl() const{
    QString url = "";

    qDebug("Sending multicast request...\n");
    QString requestMessage;
    requestMessage.append("M-SEARCH * HTTP/1.1\r\n");
    requestMessage.append("HOST: " + SSDP_IP + ":" + QString::number(SSDP_PORT) + "\r\n");
    requestMessage.append("ST: urn:schemas-upnp-org:device:InternetGatewayDevice:1\r\n");
    requestMessage.append("MAN: \"ssdp:discover\"\r\n");
    requestMessage.append("MX: 2\r\n");
    requestMessage.append("\n");

    qDebug("Request Messasge:\n");
    qDebug() << requestMessage;
    QByteArray bArray = QByteArray::fromStdString(requestMessage.toStdString());
    //QByteArray bArray;
    //bArray.append("M-SEARCH * HTTP/1.1\r\nHOS: 239.255.255.250:1900\r\nST: urn:schemas-upnp-org:device:InternetGatewayDevice:1\r\nMAN: \"ssdp:discover\"\r\nMX: 2\r\n\n");
    udpSocket->writeDatagram(bArray.data(), bArray.size(), *SSDP_IP_HOST, 1900);


    return url;
}

RouterFinder::~RouterFinder(){
    delete udpSocket;
    delete SSDP_IP_HOST;
}
