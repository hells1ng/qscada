#ifndef SQLDRIVER_H
#define SQLDRIVER_H

#include <QtCore/QtCore>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>


class SqlDriver : public QObject
{
    Q_OBJECT
public:
    explicit SqlDriver(QObject *parent = 0);
    ~SqlDriver();

    QString     prepare_data(const QString &str);

    void        toDataTable(const QString &data);

    QString     fromDataTable(int data_size);

signals:

public slots:

private:
    QSqlDatabase db;

    QString     get_systime();
    void        TestSqlDriver();

};

#endif // SQLDRIVER_H
