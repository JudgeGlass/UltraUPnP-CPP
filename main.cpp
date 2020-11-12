#include <QCoreApplication>
#include <QtNetwork/QNetworkDatagram>
#include <QDebug>

#include <routerfinder.h>
#include <router.h>

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

    router->portForward(3389, 3389, "192.168.86.22", "TCP");


    delete router;
    return a.exec();
}
