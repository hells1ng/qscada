#include "sqldriver.h"
#include "../https/https.h"

/*
 * https://habrahabr.ru/post/128836/
 * http://doc.qt.io/qt-5/sql-connecting.html
 */

SqlDriver::SqlDriver(QObject *parent) :
    QObject(parent)
{    
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("Safe");
    db.setUserName("root");
    db.setPassword("0000");

//    TestSqlDriver();
}

SqlDriver::~SqlDriver()
{
}

void SqlDriver::TestSqlDriver()
{
    qDebug() << "SqlDriver.get_systime() = "    << get_systime() << endl;
    qDebug() << "SqlDriver.prepare_data(guid|value|flag) = "
             << prepare_data(QString("guid|value|flag")) << endl;
//    qDebug() << "SqlDriver.prepare_value() = "
//             << prepare_data(QString("guid|value")) << endl;
    toDataTable("guid|value|flag");
    qDebug() << "SqlDriver.fromDataTable(3) = "
             << fromDataTable(3) << endl;
}

/*need get time like 20170905 14:56:30*/
QString SqlDriver::get_systime()
{
    return QDateTime::currentDateTime().toString("yyyyMMdd hh:mm:ss");
}

QString SqlDriver::prepare_data(const QString& str)
{
    QString delim("|");
    QString ret;
    QStringList qsl = str.split(delim);

    if (qsl.size() != 3)
        qFatal("SqlDriver::prepare_data() -> too few args in Qstring");

    ret = qsl[0] + delim + qsl[1] + delim + get_systime() + delim + qsl[2] + delim + QString::number(0);

    return ret;
}

void SqlDriver::toDataTable(const QString& data)
{
    bool ok = db.open();
    if (!ok) {
        qWarning() << "Cannot connect to " << db.databaseName() << endl;
        return;
    } else {
        QSqlQuery query;
        query.prepare("INSERT INTO data (string)"
                      "VALUES (:data);");
        query.bindValue(":data", prepare_data(data));

        bool b = query.exec();
        if (!b)
            qWarning() << "Cannot write to " << db.databaseName() << endl;

        db.close();
        return;
    }
}

QString SqlDriver::fromDataTable(int data_size)
{
    QString retstr(QString::fromStdString(HttpsDriver::EMPTY_STRING));
    QString delim("/");

    bool ok = db.open();
    if (!ok) {
        qWarning() << "Cannot connect to " << db.databaseName() << endl;
        return retstr;
    } else {
        QString query_str = "SELECT * FROM data LIMIT " + QString::number(data_size);
        QSqlQuery query;

        bool b = query.exec(query_str);
        if (!b)
            qWarning() << "Cannot read from " << db.databaseName() << endl;

        else if (query.size()) {
            retstr.clear();

            while (query.next()) {
                 retstr = retstr + query.value(0).toString() + delim;
            }

            query_str = "DELETE FROM data LIMIT " + QString::number(data_size);

            bool c = query.exec(query_str);
            if (!c)
                qWarning() << "Cannot delete from " << db.databaseName() << endl;
        }

        db.close();
        return retstr;
    }
}

/*
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
 */

