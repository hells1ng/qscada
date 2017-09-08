
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

const string HttpsDriver::GET_SENSOR_PERIOD = "GET_SENSOR_PERIOD";
const string HttpsDriver::EMPTY_STRING = "EMPTY";
//"HTTPS://81.177.142.88/00.php";


string Sql_driver::systime()
{
    time_t t;
    struct tm *t_m;
    t = time(NULL);
    t_m = localtime(&t);
    string pretime;
    ostringstream oss;
    oss << t_m->tm_year + 1900;
    if ((t_m->tm_mon + 1) < 10) oss << "0";
    oss << t_m->tm_mon + 1;
    if ((t_m->tm_mday) < 10) oss << "0";
    oss << t_m->tm_mday << " ";
    if ((t_m->tm_hour) < 10) oss << "0";
    oss << t_m->tm_hour << ":";
    if ((t_m->tm_min) < 10) oss << "0";
    oss << t_m->tm_min << ":";
    if ((t_m->tm_sec) < 10) oss << "0";
    oss << t_m->tm_sec;

    //pretime = "&datetime=";
    //pretime.append( oss.str() );

    pretime = oss.str();

//    cout << "Time = " << pretime << endl;
    return pretime;
}
/*--------------------------------------*/
/*EXPLODING STRING BY DEL SYMPOL(DELIM)*/
/* This funtion crete Vector with substring from Str srting by custom DEL*/
/*--------------------------------------*/
vector<string> Sql_driver::explode_string (string str, char delim)
{
    //size_t prev = 0;
    //size_t next;
    stringstream ss(str);
    string item;
    vector<string> arr;

    while (getline(ss, item, delim)) {
        arr.push_back(item);
    }
    return arr;
}

/*------------------------*/
/*  DATA PREPARATION------*/
/*here we build string with all nessesary markers--> id,value,flag, datetime etc.*/
/*------------------------*/
string Sql_driver::Preparedata(string data)
{
    string postmsg;
    vector <string> arr = explode_string(data, '|');
    /*
     *  arr[0] - id, arr[1] - value, arr[2] - flag
     */
    //postmsg = "id=" + arr[0] + "&value=" + arr[1] + "&value_flag=" + arr[2] + "&error_flag=0" + systime();
    //postmsg = data + "|" + "0" + "|" + systime();

    postmsg = arr[0] + "|" + arr[1] + "|" + systime() + "|" + arr[2] + "|" + "0";
/*
    postmsg = "{\"insert_1\": {\"to_do\": \"insert_signal_value\", \"target\": {\"sensor_value\": \"" + arr[0] +
                "\", \"value_signal\": \"" + arr[1] + "\", \"datetime_signal\": \"" + systime() +
                "\", \"error_flag\": \"0\", \"value_flag\": \"" + arr[2] + "\"}}}";
*/
    return postmsg;
}



int Sql_driver::toData(string str)
{
    MYSQL *con = mysql_init(NULL);
    char qurr[4096];
    sprintf(qurr,"insert into data(string)VALUES('%s')", str.c_str());
    if (mysql_real_connect(con, "localhost", "root", "0000",  "Safe", 0, NULL, 0) == NULL){
        fprintf(stderr, "%s\n", mysql_error(con));
        cout << "Couldn't connect to SQL: " << stderr << endl;
    }
    else {
        mysql_query(con, qurr);
    }
    mysql_close(con);
    return 0;
}
/*--------------------------------------------------------*/
Sql_driver::Sql_driver()
{
}

Sql_driver::~Sql_driver()
{
}

string Sql_driver::fromData(int datasize)
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    MYSQL *con = mysql_init(NULL);
    string name(HttpsDriver::EMPTY_STRING);
    std::string size_ = std::to_string(datasize);

    if (mysql_real_connect(con, "localhost", "root", "0000",  "Safe", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        cout << "Couldn't connect to SQL: " << stderr << endl;
    }
    else
    {
        string query = string("SELECT * FROM data limit ") + size_;
        mysql_query(con, query.c_str());
        result = mysql_store_result(con);

        if (mysql_num_rows(result))
        {
            row = mysql_fetch_row(result);
            name = row[0];
            while ((row = mysql_fetch_row(result)) != NULL)
                name = name + "/" + row[0];

            size_ = std::to_string(mysql_num_rows(result));
            query = string("DELETE FROM data limit ") + size_;

            if (mysql_query(con, query.c_str()))
                cout << "DELETED" << endl;
        }
        else
            name = HttpsDriver::EMPTY_STRING;

        mysql_free_result(result);
    }

    mysql_close(con);

    return name;
}

vector<string> Sql_driver::fromPost()
{
    vector<string> arr;
    MYSQL_RES *result;
    MYSQL *con = mysql_init(NULL);
    MYSQL_ROW row;

    if (mysql_real_connect(con, "localhost", "root", "0000", "Safe", 0, NULL, 0) == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        cout << "connection to SQL is lost Sql_driver::fromPost" << endl;
        mysql_close(con);
        return arr;
    }
    else {
        mysql_query(con, "SELECT id,guid,value,status FROM post where status=0");
        result = mysql_use_result(con);

        if((row = mysql_fetch_row(result)) != NULL) {
            arr.push_back(row[1]); //guid
            arr.push_back(row[2]); //value
            arr.push_back(row[3]); //status

            cout << "FROM Post: Guid = " << arr[0] << " Value = " << arr[1] << " Status = " << arr[2] << endl;
        }
        mysql_free_result(result);//for next mysql_query
        mysql_close(con);
    }
    return arr;
}

void Sql_driver::toPost(string guid, bool status)
{
    MYSQL *con = mysql_init(NULL);
    char qurr[200];

    if (mysql_real_connect(con, "localhost", "root", "0000", "Safe", 0, NULL, 0) == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        cout << "connection to SQL is lost Sql_driver::toPost" << endl;
        mysql_close(con);
        //return;
    }
    else {
        if (status) {
            cout << "WRITED SUCCESS from Post base" << endl;
            sprintf(qurr,"UPDATE post set status=1 where guid='%s'\n", guid.c_str());
        }
        else {
            cout << "Updated with ERROR from Post base" << endl;
            sprintf(qurr,"UPDATE post set status=2 where guid='%s'\n", guid.c_str());
        }
        mysql_query(con, qurr);
        mysql_close(con);
    }
}

/* Функция возвращает из строки, состоящей из элементов,
 * соединенных через разделитель, вектор этих элементов
 */
vector<string> HttpsDriver::splitString(const string &fullstr,
                                  const string &delimiter) {
    vector<string> elements;
    string::size_type lastpos = fullstr.find_first_not_of(delimiter, 0);
    string::size_type pos = fullstr.find_first_of(delimiter, lastpos);
    string substring("");

    while ((string::npos != pos) || (string::npos != lastpos)) {

        substring = fullstr.substr(lastpos, pos - lastpos);
        elements.push_back(substring);
        substring = "";

        lastpos = fullstr.find_first_not_of(delimiter, pos);
        pos = fullstr.find_first_of(delimiter, lastpos);
    }
    return elements;
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

QtJson::JsonObject Sql_driver::from_data_table_to_json(string data) {
    vector <string> arr = explode_string(data, '|');


    QtJson::JsonObject target, insert;
    target["sensor_secret"] = arr[0].c_str();
    target["value_signal"]  = arr[1].c_str();
    target["datetime_signal"] = arr[2].c_str();
    target["value_flag"] = arr[3].c_str();
    target["error_flag"] = arr[4].c_str();

    insert["to_do"] = "insert_signal_value";
    insert["token"] = "";
    insert["target"] = target;
    insert["filter"] = "";
    insert["fields"] = "";
//    QByteArray in = QtJson::serialize(insert);
//    qDebug() << "JSONinsert= " << in.data();
    return insert;

//    QtJson::JsonObject json;
//    string ins = "insert_" + std::to_string(number);
//    json[ins.c_str()] = insert;
//    QByteArray ret = QtJson::serialize(json);
//    qDebug() << "JSONstr = " << ret.data();
//    return ret.data();
}

size_t CurlWriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
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

int HttpsDriver::Send(string data) {
    if (data == EMPTY_STRING) return 1;

    int ret = 0;
    Sql_driver SQL_dr;
    vector<string> datas;
    string request_str;
    QtJson::JsonObject json, param;

    if (data == GET_SENSOR_PERIOD)
    {
        string ins = "check_interval";
        json[ins.c_str()] = get_interval_json();
    }
    else
    {
        datas = splitString(data, "/");

        for (uint i = 0; i < datas.size(); i++)
        {
            string ins = "insert_" + std::to_string(i);
            json[ins.c_str()] = SQL_dr.from_data_table_to_json(datas[i]);
        }
    }

    param["parameters"] = json;

    request_str = QtJson::serialize(param).data();

    string response_str = curl_send(request_str);

    process_response(data, response_str, datas);

    return ret;
}


QString HttpsDriver::process_response(string const &data, string const &response, vector<string> &datas) {
    vector<string> str;
    QtJson::JsonObject result;
    Sql_driver SQL_dr;

    if (data == GET_SENSOR_PERIOD)
    {
        if (response == EMPTY_STRING)   //no connection
        {
            return QString::fromStdString(EMPTY_STRING);
        }
        else
        {
            bool ok;
            result = QtJson::parse(QString::fromStdString(response), ok).toMap();
            if(!ok)
                qFatal("An error occurred during parsing");

            string ins = "check_interval";
            QtJson::JsonObject nested = result[ins.c_str()].toMap();
            //            qDebug() << "value_option:" << nested["value_option"].toInt();
            //            qDebug() << "results:" << nested["results"].toInt();

            return nested["results"].toString();
        }
    }
    else
    {
        if (response == EMPTY_STRING)   //no connection
        {
            for (uint i = 0; i < datas.size(); i++)
            {
                str = splitString(datas[i], "|");
                SQL_dr.toData(str[0] + "|" + str[1] + "|" + str[2] + "|" + str[3] + "|" + "1");
            }
        }
        else    //get answer from server
        {
            bool ok;
            result = QtJson::parse(QString::fromStdString(response), ok).toMap();
            if(!ok)
                qFatal("An error occurred during parsing");

            for (uint i = 0; i < datas.size(); i++)
            {
                //                found = retdata[i].find(findstring);//Ищем SUCCESS
                //                if (found == std::string::npos) {
                //                    str = splitString(senddata[i], "|");
                //                    SQL_dr.toData(str[0] + "|" + str[1] + "|" + str[2] + "|" + str[3] + "|" + "2");
                //                }
                string ins = "insert_" + std::to_string(i);
                QtJson::JsonObject nested = result[ins.c_str()].toMap();
                if (nested["results"] != "true")
                {
                    str = splitString(datas[i], "|");
                    SQL_dr.toData(str[0] + "|" + str[1] + "|" + str[2] + "|" + str[3] + "|" + "2");
                }
                //                qDebug() << "errors:" << nested["errors"].toInt();
                //                qDebug() << "results:" << nested["results"].toBool();
            }
        }
        return QString::fromStdString(EMPTY_STRING);
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

        string ret(chunk.memory, chunk.size);
        retstr = ret;

        if (res == CURLE_OK) //если соединение успешно
        {
            qDebug() << "cURL returns : " << QString::fromStdString(ret) << endl;;
        }
        else // если не было соединения
        {
            retstr = EMPTY_STRING;
//            fprintf(stderr, "curl_easy_perform() failed: %s OR received not SUCCESS\n", curl_easy_strerror(res));
//            cout << stderr << endl;
            qDebug() << "cURL connection Failed!" << endl;

        }
    }
    curl_easy_cleanup(curl);
    free(chunk.memory);
    curl_global_cleanup();

    return retstr;
}

