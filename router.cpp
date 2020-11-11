#include "router.h"

Router::Router(QObject *parent) : QObject(parent)
{

}

Router::Router(const QString descriptorURL){
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(readXML(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl(descriptorURL)));

}

void Router::readXML(QNetworkReply * reply){
    QDomDocument xmlDom;
    xmlDom.setContent(reply->readAll());

    QDomElement root = xmlDom.documentElement();

    QDomNodeList routerServices = root.elementsByTagName("service");

    QString tmpServiceType;
    QString tmpControlURL;

    for(int i = 0; i < routerServices.size(); i++){
        QDomNode service = routerServices.at(i);
        QDomNodeList childrenNodes = service.childNodes();
        for(int j = 0; j < childrenNodes.size(); j++){
            QDomNode item = childrenNodes.at(j);
            if(item.nodeName() == "serviceType"){
                tmpServiceType = item.firstChild().nodeValue();
            }

            if(item.nodeName() == "controlURL"){
                tmpControlURL = item.firstChild().nodeValue();
            }
        }

        if(tmpServiceType.toLower().contains(":wanipconnection:")){
            this->serviceType = tmpServiceType;
            this->controlURL = tmpControlURL;
            break;
        }
    }

    qDebug() << "CONTROL URL: " << this->controlURL << "\n";
    qDebug() << "SERVICE TYPE: " << this->serviceType << "\n";
}
