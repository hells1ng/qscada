#ifndef MAIN_H
#define MAIN_H

#include "includes.h"

enum CommandLineParseResult
{
    CommandLineOk,
    CommandLineError,
    CommandLineVersionRequested,
    CommandLineHelpRequested
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, QString *errorMessage)
{

    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    QCommandLineOption modeOption(QStringList() << "m" << "mode",
                                  QCoreApplication::translate("main", "Start or Stop Application"),
                                  QCoreApplication::translate("main", "start/stop"));
    parser.addOption(modeOption);

    const QCommandLineOption helpOption = parser.addHelpOption();
    const QCommandLineOption versionOption = parser.addVersionOption();

    if (!parser.parse(QCoreApplication::arguments())) {
        *errorMessage = parser.errorText();
        return CommandLineError;
    }
    if (parser.optionNames().isEmpty()) {
        *errorMessage = "Emtpy args!";
        return CommandLineError;
    }

    if (parser.isSet(versionOption))
        return CommandLineVersionRequested;

    if ((parser.isSet(helpOption)))
        return CommandLineHelpRequested;

    if (parser.isSet(modeOption)) {
        const QString mode = parser.value(modeOption);
        if ((mode == QString("start")) || (mode == QString("stop"))) {
//            qDebug() << "mode = " << mode;
        }
        else
        {
            *errorMessage = "Bad mode: " + mode ;
            return CommandLineError;
        }
    }

    const QStringList positionalArguments = parser.positionalArguments();
//    if (positionalArguments.isEmpty()) {
//        *errorMessage = "Empty/bad args!";
//        return CommandLineError;
//    }
    if (positionalArguments.size() > 0) {
        *errorMessage = "Bad Args!";
        return CommandLineError;
    }
//    query->name = positionalArguments.first();

    return CommandLineOk;
}
#endif //MAIN_H
