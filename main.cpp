#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server;
//    server.listen(QHostAddress::Any, 1234); // local
    server.listen(QHostAddress::Any, 4321); // global

    return a.exec();
}
