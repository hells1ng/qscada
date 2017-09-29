#include "includes.h"
#include "main.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ThreadManager Manager;

    return a.exec();
}

