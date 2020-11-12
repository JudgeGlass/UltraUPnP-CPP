#ifndef ROUTERFINDER_H
#define ROUTERFINDER_H

#include <QObject>
#include <QString>
#include <QUdpSocket>
#include <QHostAddress>

#include <router.h>

class RouterFinder : public QObject
{
    Q_OBJECT
public:
    explicit RouterFinder(QObject *parent = 0);
    ~RouterFinder();

    bool search();
    void portForward(int internalPort, int externalPort, QString host, QString proto);

    QString getDescriptorURL() const;

    bool isConnected = false;
    void read();

signals:

public slots:


private:
    const QString SSDP_IP = "239.255.255.250";
    const QHostAddress *SSDP_IP_HOST;
    const uint16_t SSDP_PORT = 1900;
    const uint16_t SSDP_SEARCH_PORT = 1901;
    QUdpSocket *udpSocket;

    QString descriptorURL;
};

#endif // ROUTERFINDER_H
