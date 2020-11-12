#include "router.h"

Router::Router(QObject *parent) : QObject(parent)
{

}

Router::Router(const QString descriptorURL)
    : descriptorURL(descriptorURL)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply *xmlDoc;

    xmlDoc = manager->get(QNetworkRequest(QUrl(descriptorURL)));
    QEventLoop loop;
    connect(xmlDoc, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    xmlDoc->deleteLater();
    readXML(xmlDoc);

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
            this->controlURL = descriptorURL.mid(0, descriptorURL.indexOf("/", 7)) + tmpControlURL;
            break;
        }
    }

    qDebug() << "CONTROL URL: " << this->controlURL << "\n";
    qDebug() << "SERVICE TYPE: " << this->serviceType << "\n";
}


QString Router::getExternalIPAddress(){
    QList<RouterArgument> routerArguments;
    routerArguments << RouterArgument("NewExternalIPAddress", "ExternalIPAddress");

    QList<RouterArgument> response = sendCommand("GetExternalIPAddress", routerArguments);
    for(RouterArgument ra: response){
        if(ra.getArgName() == "NewExternalIPAddress"){
            return ra.getArgValue();
        }
    }

    return "COULD NOT FIND EXTERNAL IP";
}

QList<RouterArgument> Router::sendCommand(QString action, QList<RouterArgument> &arguments) {
    QList<RouterArgument> response;

    QString SOAPData = "<?xml version=\"1.0\"?>\r\n<SOAP-ENV:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n";
    SOAPData.append("<SOAP-ENV:Body>\r\n");
    SOAPData.append("<m:" + action + " xmlns:m=\"" + serviceType + "\">\r\n");

    for(int i = 0; i < arguments.size(); i++){
        QString line = QString("<%1>%2</%1>").arg(arguments.at(i).getArgName(), arguments.at(i).getArgValue());
        SOAPData.append(line);
    }



    QByteArray data;
    QString message;
    message.append("POST " + QUrl(controlURL).path() + " HTTP/1.1\n");
    message.append("SOAPAction: \"" + serviceType + "#" + action + "\"\n");
    message.append("Content-Type: text/xml\n");
    message.append("Connection: Close\n");
    message.append("User-Agent: Qt/C++ UltraUPNP\n");
    message.append("Host: " + QUrl(controlURL).host() + ":" + QString::number(QUrl(controlURL).port()) + "\n");
    message.append("Accept: text/html, image/gif, image/jpeg, *; q=.2, */*; q=.2\n");
    message.append("Content-Length: " + QString::number(SOAPData.length()) + "\n");
    message.append("\n");
    message.append(SOAPData);

    QByteArray messageArray = QByteArray::fromStdString(message.toStdString());

    QUrl url;
    url.setHost(QUrl(controlURL).host());
    url.setPort(QUrl(controlURL).port());
    url.setPath(QUrl(controlURL).path());

    url.setScheme("http");
    QNetworkRequest request = QNetworkRequest(url);

    request.setRawHeader("Content-Type", "text/xml");
    QString saction = QString("\"%1#%2\"").arg(serviceType, action);
    request.setRawHeader("SOAPAction", saction.toLocal8Bit());
    request.setRawHeader("Connection", "Close");
    request.setRawHeader("Content-Length", QString::number(SOAPData.length()).toLocal8Bit());

    QNetworkReply *res;
    networkManager = new QNetworkAccessManager();
    networkManager->connectToHost(QUrl(controlURL).host(), QUrl(controlURL).port());

    QEventLoop loop;
    res = networkManager->post(request, SOAPData.toLocal8Bit());
    connect(res, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    res->deleteLater();
    QByteArray byteResponse =  res->readAll();
    response = readRouterResponse(action, byteResponse);
    networkManager->disconnect();




    int responseCode = res->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "Response Code: " << responseCode;
    if(responseCode != 200){
        qInfo() << "There was an error with your request! (This is normal for GetPortMappings list)";
    }

    res->disconnect();
    delete res;
    delete networkManager;

    return response;
}



QList<RouterArgument> Router::readRouterResponse(QString &action, QByteArray &response){
    QList<RouterArgument> responses;
    QDomDocument xmlDom;
    xmlDom.setContent(response);

    QDomElement root = xmlDom.documentElement();

    QDomNodeList routerResponses = root.elementsByTagName("u:" + action + "Response");

    for(int i = 0; i < routerResponses.size(); i++){
        QDomNodeList childNodes = routerResponses.at(i).childNodes();
        for(int j = 0; j < childNodes.size(); j++){
            QDomNode item = childNodes.at(j);
            responses << RouterArgument(item.nodeName(), item.childNodes().at(0).nodeValue());
        }
    }

    return responses;
}

QList<RouterArgument> Router::getPortMappings()
{
    int mappingIndex = 0;
    QList<RouterArgument> routerArguments;
    routerArguments << RouterArgument("NewPortMappingIndex", QString::number(mappingIndex));

    QList<RouterArgument> routerResponse;
    QList<RouterArgument> portMappings;

    while((routerResponse = sendCommand("GetGenericPortMappingEntry", routerArguments)).size() != 0){
        routerArguments.clear();
        routerArguments << RouterArgument("NewPortMappingIndex", QString::number(mappingIndex));

        portMappings += routerResponse;

        mappingIndex++;
    }

    return portMappings;
}

void Router::portForward(int internalPort, int externalPort, QString host, QString proto)
{
    qInfo() << "Attempting: " << getExternalIPAddress() << ":" << externalPort << " --> " << host << ":" << internalPort;
    QList<RouterArgument> routerArguments;
    routerArguments << RouterArgument("NewRemoteHost", "");
    routerArguments << RouterArgument("NewProtocol", proto);
    routerArguments << RouterArgument("NewInternalClient", host);
    routerArguments << RouterArgument("NewExternalPort", QString::number(externalPort));
    routerArguments << RouterArgument("NewInternalPort", QString::number(internalPort));
    routerArguments << RouterArgument("NewPortMappingDescription", "UltraUPnP-QT");
    routerArguments << RouterArgument("NewLeaseDuration", "0");
    sendCommand("AddPortMapping", routerArguments).clear();
    routerArguments.clear();
}

void Router::removeMapping(int externalPort, QString host, QString proto)
{
    QList<RouterArgument> routerArguments;
    routerArguments << RouterArgument("NewInternalHost", host);
    routerArguments << RouterArgument("NewExternalPort", QString::number(externalPort));
    routerArguments << RouterArgument("NewProtocol", proto);
    sendCommand("DeletePortMapping", routerArguments).clear();
    routerArguments.clear();
}




















