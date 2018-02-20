#ifndef SQLDRIVER_H
#define SQLDRIVER_H

#include <QtCore/QtCore>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include "../../defines.h"
#include "../../libs/id/id.h"

class SqlDriver : public QObject
{
    Q_OBJECT
public:
    explicit SqlDriver(QObject *parent = 0);
    ~SqlDriver();

    void toDataTable(const Data &data);

    Data fromDataTable(quint16 data_size);

    int fromGuidTable(Data *retData, const QString& table, const QString &key="");

    void push(Data data);
    Data pop(const quint8 size);

signals:

public slots:

private:
//    QMutex* mutex;
    QSqlDatabase db;
    QQueue<QStringList> queue;

    QString     get_systime();
    void        TestSqlDriver();
    static qint64  numOfConnections;

};

#endif // SQLDRIVER_H
