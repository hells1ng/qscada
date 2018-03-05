#include "sqldriver.h"
#include "../https/https.h"

/*
 * https://habrahabr.ru/post/128836/
 * http://doc.qt.io/qt-5/sql-connecting.html
 */
qint64 SqlDriver::numOfConnections = 0;

SqlDriver::SqlDriver(QObject *parent) :
    QObject(parent)
{
//    mutex = new QMutex();

//    numOfConnections++;
    if(QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        db = QSqlDatabase::database();
    } else {
//        QSqlDatabase defDB = QSqlDatabase::addDatabase("QMYSQL", QString::number(numOfConnections));
//        db = QSqlDatabase::database(QString::number(numOfConnections));
//        db = QSqlDatabase::addDatabase("QMYSQL");
//        db.setDatabaseName("Safe");

        db = QSqlDatabase::addDatabase("QSQLITE");
//        db.setDatabaseName("qscada_db");
        db.setDatabaseName("/home/pi/qscada_db");
//        QStringList tables = db.tables();
//        qDebug() << "Tables: " << tables;
//        db.setDatabaseName("qscada_db");

    }
//    db.setHostName("localhost");
//    db.setUserName("root");
//    db.setPassword("0000");

//    qDebug() << QString::number(numOfConnections) << endl;

//    TestSqlDriver();
}

SqlDriver::~SqlDriver()
{
}
void SqlDriver::push(Data data)
{
//    foreach (const QStringList &qsl, data) {
//        qDebug() << "Push = " << qsl;
//    }
//    QStringList *qsl = data.data();
//    qDebug() << qsl->size() ;

    for(int i = 0; i < data.size(); i++) {

        if (data[i].size() != DATA_NUM_OF_ELEM_START) {
            qFatal("SqlDriver::push() -> too few args in QstringList");

        } else {
            data[i].append(get_systime());
            data[i].append(QString::number(DATA_ERROR_FLAG0));
        }
//        mutex->lock();
        queue.enqueue(data[i]);
//        mutex->unlock();
    }
}


Data SqlDriver::pop(const quint8 size)
{
    Data retData;
    QStringList ret;

    quint8 i = 0;

    while (!queue.empty() && (i < size)) {
       ret = queue.dequeue();
//       qDebug() << "Pop = " << ret;
       retData.append(ret);
       i++;
    }
    return retData;
}

void SqlDriver::TestSqlDriver()
{
//    qDebug() << "SqlDriver.get_systime() = "    << get_systime() << endl;
//    qDebug() << "SqlDriver.prepare_data(guid|value|flag) = "
//             << prepare_data(QString("guid|value|flag")) << endl;
//    qDebug() << "SqlDriver.prepare_value() = "
//             << prepare_data(QString("guid|value")) << endl;
//    toDataTable("guid|value|flag");
//    qDebug() << "SqlDriver.fromDataTable(3) = "
//             << fromDataTable(3) << endl;

}

/*need get time like 20170905 14:56:30*/
QString SqlDriver::get_systime()
{
    return QDateTime::currentDateTime().toString("yyyyMMdd hh:mm:ss");
}

/*
    Функция для сохранения данных при ошибках передачи на сервер
*/
void SqlDriver::toDataTable(const Data& data)
{
//    foreach (const QStringList &qsl, data) {
//        qDebug() << "toDataTable = " << qsl;
//    }
    bool ok = db.open();
    if (!ok) {
        qWarning() << "Cannot connect to " << db.databaseName() << endl;
        return;
    } else {
        for (int i = 0; i < data.size(); i++) {
            QStringList slist = data.at(i);
            //сохраняем только с не DATA_ERROR_FLAG0
            if (slist.at(DATA_POS_ERRORFLAG) != QString::number(DATA_ERROR_FLAG0)) {

//                qDebug() << "Write to data Table " << slist << endl;

                QSqlQuery query;
                query.prepare("INSERT INTO Data (guid,value,value_flag,time,error_flag)"
                              "VALUES (:guid,:value,:value_flag,:time,:error_flag);");

                query.bindValue(":guid",        slist.at(DATA_POS_GUID));
                query.bindValue(":value",       slist.at(DATA_POS_VALUE));
                query.bindValue(":value_flag",  slist.at(DATA_POS_VALUEFLAG));
                query.bindValue(":time",        slist.at(DATA_POS_TIME));
                query.bindValue(":error_flag",  slist.at(DATA_POS_ERRORFLAG));

                bool b = query.exec();
                if (!b) {
                    qWarning() << "Cannot write to " << db.databaseName() << endl;
//                    qWarning() << "Error =  " << query.e << endl;
                    qDebug() << "SqLite error:" << query.lastError().text();
                }
            }
        }

        db.close();
        return;
    }
}
/*
    Функция для вынимания данных из базы хранения непереданных данных
*/
Data SqlDriver::fromDataTable(quint16 data_size)
{
    Data retData;

    bool ok = db.open();
    if (!ok) {
        qWarning() << "fromDataTable: Cannot connect to " << db.databaseName() << endl;
    } else {
        QString query_str = "SELECT * FROM data LIMIT " + QString::number(data_size);
        QSqlQuery query;

        bool b = query.exec(query_str);
        if (!b)
            qWarning() << "Cannot read from " << db.databaseName() << endl;

        else if (query.size()) {

            while (query.next()) {

                QStringList qsl;
                const quint8 offset = 0;//post_id on 1
                qsl.append(query.value(offset + DATA_POS_GUID).toString());
                qsl.append(query.value(offset + DATA_POS_VALUE).toString());
                qsl.append(query.value(offset + DATA_POS_VALUEFLAG).toString());
                qsl.append(query.value(offset + DATA_POS_TIME).toString());
                qsl.append(query.value(offset + DATA_POS_ERRORFLAG).toString());

                retData.append(qsl);
//                post_id_vector.append(query.value(0).toString());

                //удаляем строку
                QSqlQuery query_delete;
                query_delete.prepare("DELETE FROM Data WHERE guid=:guid AND value=:value AND "
                              "value_flag=:value_flag AND time=:time AND error_flag=:error_flag ;");

                query_delete.bindValue(":guid",        qsl.at(DATA_POS_GUID));
                query_delete.bindValue(":value",       qsl.at(DATA_POS_VALUE));
                query_delete.bindValue(":value_flag",  qsl.at(DATA_POS_VALUEFLAG));
                query_delete.bindValue(":time",        qsl.at(DATA_POS_TIME));
                query_delete.bindValue(":error_flag",  qsl.at(DATA_POS_ERRORFLAG));

                bool d = query_delete.exec();

//                qDebug()<< query_delete.lastQuery();

                if (!d) {
                    qWarning() << "Cannot delete from " << db.databaseName() << endl;
                    qDebug() << "SqLite error:" << query.lastError().text();
                }
            }
        }

        db.close();
    }
    return retData;
}

int SqlDriver::fromGuidTable(Data* retData, const QString& table, const QString& key)
{
//    Data retData;
    QString query_str;
    QSqlQuery query;
    QStringList qsl;
    //TODO : без этой строчки крашатся гуайдишники на малине, WTF?????
    //с первой крашатся на малине, со второй крашатся в куте

    bool ok = false;
         ok = db.open();

    if (!ok) {
        qWarning() << "fromGuidTable: Cannot connect to " << db.databaseName() << endl;
        qFatal("Exit");
    } else {


        query_str = "SELECT * FROM " + table;
        if (key != "")
            query_str = "SELECT * FROM " + table + " WHERE key=" + key;

        bool b = query.exec(query_str);
        if (!b) {
            qWarning() << "fromGuidTable: Cannot read from DB:" << db.databaseName()
                       << " and Table: " << table << endl;
//            qFatal("Exit");
        }
        else
        {
            while (query.next()) {

                qsl.append(query.value(GuidClass::POS_KEY).toString());
                qsl.append(query.value(GuidClass::POS_GUID).toString());
                qsl.append(query.value(GuidClass::POS_ADDRESS).toString());

                retData->append(qsl);
                qsl.clear();
            }
        }
        query.clear();
        db.close();
    }

//    for (int i = 0; i < 5; i++)
//    {
//        qsl.append(QString::number(i));
//        QString a("P");
//        a = a + QString::number(i);
//        qsl.append(a);
//        qsl.append(QString::number(i));
//        retData->append(qsl);
//        qsl.clear();
//    }
    return 1;
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

