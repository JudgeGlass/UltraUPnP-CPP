#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHostAddress>
#include <QXmlStreamReader>
#include <QTemporaryFile>
#include <QDomDocument>
#include <QDomNodeList>
#include <QList>
#include <QEventLoop>

#include <routerargument.h>

class Router : public QObject
{
    Q_OBJECT
public:
    const QString TCP = "TCP";
    const QString UDP = "UDP";

    Router(const QString descriptorURL);
    explicit Router(QObject *parent = nullptr);

    QString getExternalIPAddress();
    QList<RouterArgument> sendCommand(QString action, QList<RouterArgument> &arguments);
    QList<RouterArgument> readRouterResponse(QString &action, QByteArray &response);
    QList<RouterArgument> getPortMappings();

    void portForward(int internalPort, int externalPort, QString host, QString proto);
    void removeMapping(int externalPort, QString host, QString proto);
    void readXML(QNetworkReply * reply);
public slots:


signals:

private:
    const QString descriptorURL;
    QString controlURL;
    QString serviceType;

    QNetworkAccessManager *networkManager;
};

#endif // ROUTER_H
