#ifndef HTTPS_H
#define HTTPS_H
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <string>
#include <sstream>
#include <map>

#include <curl/curl.h>
#include <mysql/mysql.h>
#include <time.h>

#include <QtCore/QtCore>
#include "../../defines.h"
#include "../json/json.h"
#include "../sqldriver/sqldriver.h"

using namespace std;

class HttpsDriver
{
public:
    HttpsDriver();
    ~HttpsDriver();
//    vector<string> splitString(const string &fullstr, const string &delimiter);
    int Send(string);

    static const string GET_SENSOR_PERIOD;
    static const string EMPTY_STRING;
    vector<string> explode_string(string str, const char *delim);

protected:
    static const char *pPassphrase;
    static const char *pCertFile;
    static const char *pCACertFile;
    static const char *pCAPath;
    static const char *pHeaderFile;
    static const char *pKeyName;
    static const char *pCAType;
    static const char* SERVERADDR;
//    const char *pKeyType="PEM";

private:
    QtJson::JsonObject get_interval_json();
    QtJson::JsonObject from_data_table_to_json(string data);
    string curl_send(string const &str);
    QString process_response(string const &data, string const &response, vector<string> &datas);

};

#endif // HttpsDriver_H
