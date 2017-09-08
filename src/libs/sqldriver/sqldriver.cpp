#include "sqldriver.h"

SqlDriver::SqlDriver(QObject *parent) :
    QObject(parent)
{
    TestSqlDriver();
}
void SqlDriver::TestSqlDriver()
{
    qDebug() << "SqlDriver.get_systime() = "    << get_systime() << endl;
    qDebug() << "SqlDriver.prepare_value() = "  << prepare_data(QString("guid|value|flag")) << endl;
    qDebug() << "SqlDriver.prepare_value() = "  << prepare_data(QString("guid|value")) << endl;

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
    QStringList qsl = str.split('|');

    if (qsl.size() != 3)
        qFatal("SqlDriver::prepare_data() -> too few args in Qstring");

    ret = qsl[0] + delim + qsl[1] + delim + get_systime() + delim + qsl[2] + delim + QString::number(0);
    return ret;
}

void SqlDriver::toDataTable(QString data)
{
    QString datastr = prepare_data(data);
    // to be continued
}
