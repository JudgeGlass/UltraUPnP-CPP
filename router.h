#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QTemporaryFile>
#include <QDomDocument>
#include <QDomNodeList>

class Router : public QObject
{
    Q_OBJECT
public:
    Router(const QString descriptorURL);
    explicit Router(QObject *parent = nullptr);

public slots:
    void readXML(QNetworkReply * reply);
signals:

private:
    QString controlURL;
    QString serviceType;



};

#endif // ROUTER_H
