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
    void search() const;
    QString getDescriptorURL() const;
signals:

public slots:
    void read();

private:
    const QString SSDP_IP = "239.255.255.250";
    const QHostAddress *SSDP_IP_HOST;
    const uint16_t SSDP_PORT = 1900;
    const uint16_t SSDP_SEARCH_PORT = 1901;
    QUdpSocket *udpSocket;

    QString descriptorURL;

    Router *router;
};

#endif // ROUTERFINDER_H
