#ifndef MAIN_H
#define MAIN_H


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

//int write_Owen_data(ModbusClass* modbus, vector<OwenClass*> ovector, GuidClass* guidMap , string guidforwrite, int valueforwrite)
//{ //TODO реализовать запись в тредменегере
//    int ret_ = 0;
//    OwenClass *owenptr = ovector[0];

//    vector<string> stringguidforwrite = owenptr->find_guid_parameters(guidMap, guidforwrite);//addr pin guid

//    if (stringguidforwrite.size() == 0) {
//        return ret_;
//    }

//    for (unsigned int i = 0; i < ovector.size(); i++) {
//        owenptr = ovector[i];

//        if (to_string(owenptr->address) == stringguidforwrite[1]) {
//            int rc = owenptr->write_data(modbus, stringguidforwrite, valueforwrite);
//            if (rc == 1) {
//                ret_ = 1;
//            }
//        }
//    }
//    return ret_;
//};

//void FromPost()//TODO: interface func SqlDriver with Post table
//{
//    //cout << "       FROM_POST_THREAD " << endl;
//    SqlDriver Sql_post;
//    vector <string> arr;

////    arr = Sql_post.fromPost();// guid, value, status=0
//    if ((arr.size() == 3)) {
////        bool res = (write_Owen_data(&Modbus, OwenVector, &Guid, arr[0], stoi(arr[1])) == 1);
//        // true - success, false = error
////        Sql_post.toPost(arr[0], res); // guid, status
//    }
//}


#endif //MAIN_H
