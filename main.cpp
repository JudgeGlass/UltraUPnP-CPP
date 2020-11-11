#include <QCoreApplication>
#include <QtNetwork/QNetworkDatagram>
#include <QDebug>

#include <routerfinder.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RouterFinder routerFinder;

    routerFinder.search();


    return a.exec();
}
