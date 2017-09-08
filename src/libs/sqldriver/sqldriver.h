#ifndef SQLDRIVER_H
#define SQLDRIVER_H

#include <QtCore/QtCore>

//#include <QtCore/QObject>
//#include <QtCore/QDateTime>
//#include <QtCore/QDebug>

class SqlDriver : public QObject
{
    Q_OBJECT
public:
    explicit SqlDriver(QObject *parent = 0);
    ~SqlDriver() {}

    QString prepare_data(const QString &str);
    void toDataTable(QString data);

signals:

public slots:

private:
    QString get_systime();
    void TestSqlDriver();

};

#endif // SQLDRIVER_H
