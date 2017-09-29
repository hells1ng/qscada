
#include "https.h"

#define FAIL    -1

struct MemoryStruct {
  char *memory;
  size_t size;
};

HttpsDriver::HttpsDriver()
{
    //ctor
}

HttpsDriver::~HttpsDriver()
{
    //dtor
}

const char *HttpsDriver::pPassphrase = "myveryownca";
const char *HttpsDriver::pCertFile = "/etc/apache2/ssl/converted/NUC_ASVSPROJECT.pem";
const char *HttpsDriver::pCACertFile = "/etc/apache2/ssl/converted/rootCA.pem";
const char *HttpsDriver::pCAPath = "/etc/apache2/ssl/converted/";
const char *HttpsDriver::pHeaderFile = "dumpit";

const char *HttpsDriver::pKeyName = "/etc/apache2/ssl/converted/NUC_ASVSPROJECT.key";
const char *HttpsDriver::pCAType = "PEM";

const char *HttpsDriver::SERVERADDR = "https://tomatto.ddns.net:444";


/*--------------------------------------*/
/*EXPLODING STRING BY DEL SYMPOL(DELIM)*/
/* This funtion crete Vector with substring from Str srting by custom DELIM*/
/*--------------------------------------*/
vector<string> HttpsDriver::explode_string (string str, const char* delim)
{
    //size_t prev = 0;
    //size_t next;
    stringstream ss(str);
    string item;
    vector<string> arr;

    while (getline(ss, item, *delim)) {
        arr.push_back(item);
    }
    return arr;
}


QtJson::JsonObject HttpsDriver::get_interval_json() {
    /*  data = {\"to_do\": \"show_system_interval\", \"token\":\"\", \"target\": \"\",\"filter\":\"\", \"fields\":\"\"}";
    */
    QtJson::JsonObject insert;

    insert["to_do"] = "show_system_interval";
    insert["token"] = "";
    insert["target"] = "";
    insert["filter"] = "";
    insert["fields"] = "";

    return insert;
}

/*from_data_table_to_json string*/

QtJson::JsonObject HttpsDriver::from_data_to_json(QStringList data) {

//    vector <string> arr = explode_string(data, "|");

    QtJson::JsonObject target, insert;

//    target["sensor_secret"] = arr[0].c_str();
//    target["value_signal"]  = arr[1].c_str();
//    target["datetime_signal"] = arr[2].c_str();
//    target["value_flag"] = arr[3].c_str();
//    target["error_flag"] = arr[4].c_str();

    target["sensor_secret"]     = data.at(DATA_POS_GUID).toStdString().c_str();
    target["value_signal"]      = data.at(DATA_POS_VALUE).toStdString().c_str();
    target["datetime_signal"]   = data.at(DATA_POS_TIME).toStdString().c_str();
    target["value_flag"]        = data.at(DATA_POS_VALUEFLAG).toStdString().c_str();
    target["error_flag"]        = data.at(DATA_POS_ERRORFLAG).toStdString().c_str();

    insert["to_do"] = "insert_signal_value";
    insert["token"] = "";
    insert["target"] = target;
    insert["filter"] = "";
    insert["fields"] = "";

    return insert;

}

size_t CurlWriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {

    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

void HttpsDriver::Send(const quint8 cmd, Data *data) {

//    if ((data == EMPTY_STRING) || (data.size() == 0)) return 1;
    if (data->isEmpty())
         return;

    QtJson::JsonObject  json;

    if (cmd == HTTPS_CMD_GET_SENSOR_PERIOD) {

        string ins = "check_interval";
        json[ins.c_str()] = get_interval_json();

    } else if (cmd == HTTPS_CMD_POST_SENSOR_VALUE){

        quint8 i = 0;
        foreach (const QStringList &qsl, *data) {
            string ins = "insert_" + std::to_string(i);
            json[ins.c_str()] = from_data_to_json(qsl);
            i++;
        }
    }

    QtJson::JsonObject param;
    param["parameters"] = json;

    string response_str = curl_send( QtJson::serialize(param).data() );

    process_response(cmd, data, response_str);

}


void HttpsDriver::process_response(const quint8 cmd, Data *data, string const &response/*, vector<string> &datas*/)
{
    vector<string> str;
    QtJson::JsonObject result;
    static SqlDriver SQL_dr;
    QStringList qsl;

    if (cmd == HTTPS_CMD_GET_SENSOR_PERIOD) { // TODO this section if

        if (response.empty()) {   //no connection

//            return QString::fromStdString(EMPTY_STRING);
//            return qsl;
            return;
        } else {
            bool ok;
            result = QtJson::parse(QString::fromStdString(response), ok).toMap();
            if(!ok)
                qFatal("An error occurred during parsing response from server");

            string ins = "check_interval";
            QtJson::JsonObject nested = result[ins.c_str()].toMap();
            //            qDebug() << "value_option:" << nested["value_option"].toInt();
            //            qDebug() << "results:" << nested["results"].toInt();

//            return nested["results"].toString();
            qsl.append(nested["results"].toString());
//            return qsl;
            return;
        }

    } else if (cmd == HTTPS_CMD_POST_SENSOR_VALUE) {

        if (response.empty()) {  //no connection

            for (int i = 0; i < data->size(); i++) {
                /*Update Error flag*/
                QStringList slist = data->at(i);
                slist.replace(DATA_POS_ERRORFLAG, QString::number(DATA_ERROR_FLAG1));
                data->replace(i, slist);

//                foreach (const QStringList &qsl, *data) {
//                    qDebug() << "Https::Send -> no connect" << qsl;
//                }
            }

        } else {   //get answer from server
            bool ok;
            result = QtJson::parse(QString::fromStdString(response), ok).toMap();

            if(!ok)
                qFatal("An error occurred during parsing response from server");

            for (int i = 0; i < data->size(); i++) {

                string ins = "insert_" + std::to_string(i);
                QtJson::JsonObject nested = result[ins.c_str()].toMap();
                QtJson::JsonObject errors = nested["errors"].toMap();

                if (nested["results"] != "true") {
                    /*Update Error flag*/
                    if ((errors["code"] == HTTP_RESPONSE_ERROR_DUPLICATE) ||
                        (errors["code"] == HTTP_RESPONSE_ERROR_WRONGSECRET)) {
                        data->remove(i);
//                        qDebug() << "Deleted duplicate request!" << endl;
                    }
                    else {
                        QStringList slist = data->at(i);
                        slist.replace(DATA_POS_ERRORFLAG, QString::number(DATA_ERROR_FLAG2));
                        data->replace(i, slist);
                    }
                }

//                foreach (const QStringList &qsl, *data) {
//                    qDebug() << "Https::Send -> get answer" << qsl;
//                }
            }
        }
    }
}

string HttpsDriver::curl_send(string const &str) {
    //    qDebug() << QString::fromStdString(str);

    struct MemoryStruct chunk;
    string retstr;
    CURL *curl;

    chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;                     /* no data at this point */

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl)
    {
        CURLcode res;
        /*SetOPT is the settings*/
       //curl_easy_setopt(curl, CURLOPT_URL, "HttpsDriver://ca.asvs.ru");
        curl_easy_setopt(curl, CURLOPT_URL, SERVERADDR);
        /* send all data to this function  */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteMemoryCallback);
        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
       // curl_easy_setopt(curl, CURLOPT_HEADERDATA, headerfile);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);        //VEBOSE is the DEBUG MODE write 1L instead to swith ON DEBUG MODE

        /*
        curl_easy_setopt(curl, CURLOPT_CAPATH, pCAPath);        //pCAPath is the folder where certificates are stored
        curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");     //CertType .pem for Linux .crt for Win
        curl_easy_setopt(curl, CURLOPT_SSLCERT, pCertFile);     // SSL cert path
        curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, pKeyType);
        curl_easy_setopt(curl, CURLOPT_SSLKEY, pKeyName);         //Key to cert path
        curl_easy_setopt(curl, CURLOPT_CAINFO, pCACertFile) ;      //ROOT Cert, use it when u need vise verce authority
        //curl_easy_setopt(curl, CURLOPT_WRITEDATA, retcurl);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);         //Enable to verify yourself cert with rootCA
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L);         //Enable to veryfy host(servert) cert
        */

//        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);         //Buffer is the POST MESSAGE
        struct curl_slist *headers = NULL;
        //headers = curl_slist_append(headers, "Content-Type: application/json");
        //headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        headers = curl_slist_append(headers, "charsets: utf-8");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str.c_str());

        //curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

        //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, &post_fields);
        //curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_fields.length());

        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);         //Waiting for connection 5sec
        /* complete within 20 seconds */
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        res = curl_easy_perform(curl);

        string retFromServer(chunk.memory, chunk.size);

        if (res == CURLE_OK) //если соединение успешно
        {
            retstr = retFromServer;
            qDebug() << "cURL returns : " << QString::fromStdString(retFromServer) << endl;;
        }
        else // если не было соединения
        {
            retstr = "";
            qDebug() << "cURL connection Failed!" << endl;
        }
    }
    curl_easy_cleanup(curl);
    free(chunk.memory);
    curl_global_cleanup();

    return retstr;
}

