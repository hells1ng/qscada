#include "includes.h"
#include "main.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("ACBC SCADA");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;

    QString errorMessage;
    switch (parseCommandLine(parser, &errorMessage)) {
    case CommandLineOk:
        break;
    case CommandLineError:
        fputs(qPrintable(errorMessage), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
        return 1;
    case CommandLineVersionRequested:
        printf("%s %s\n", qPrintable(QCoreApplication::applicationName()),
               qPrintable(QCoreApplication::applicationVersion()));
        return 0;
    case CommandLineHelpRequested:
        parser.showHelp();
        Q_UNREACHABLE();
    }

    ThreadManager Manager;
    QObject::connect(&Manager, &ThreadManager::finish, &app, &QCoreApplication::quit);

//    qDebug() << parser.values(targetDirectoryOption);

//    app.exec();


    return app.exec();

}

