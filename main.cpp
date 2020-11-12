#include <QCoreApplication>
#include <QtNetwork/QNetworkDatagram>
#include <QDebug>
#include <QList>

#include <routerfinder.h>
#include <router.h>
#include <routerargument.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RouterFinder routerFinder;

    if(routerFinder.search()){
        qDebug() << "CONNECTED";
    }else{
        qDebug() << "Could not connect";
        return -1;
    }

    Router *router = new Router(routerFinder.getDescriptorURL());
    qDebug() << "External IP: " << router->getExternalIPAddress();

    QList<RouterArgument> portMappings = router->getPortMappings();
    for(RouterArgument ra: portMappings){
        qInfo() << "<" << ra.getArgName() << ">" << ra.getArgValue() << "</" << ra.getArgName() << ">\n";
    }


    delete router;
    return a.exec();
}
