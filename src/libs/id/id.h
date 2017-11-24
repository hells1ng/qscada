#ifndef ID_H_INCLUDED
#define ID_H_INCLUDED

#include <QtCore/QtCore>
#include <QtCore/QDebug>

#include "../../defines.h"
#include "../../libs/sqldriver/sqldriver.h"

class SqlDriver;

class GuidClass
{
public:
//    typedef QVector<QStringList> Guid;
//    typedef QVector<QMap<QString, QString>> SubGuid;
    typedef struct
    {
        QVector<QStringList>                     main_guid;  //vector of qsl for pair "guid - device adress"
        QVector<QMap<QString,QString>>           sub_guid;   //vector of map for pair "subguid - pin address of device"
        QVectorIterator<QStringList>*            main_it;
        QVectorIterator<QMap<QString, QString>>* sub_it;
        QStringList                              current_guid;    //qsl for pair "guid - address"
        QMap<QString, QString>                   current_subguid; //map for pair "subguid - pin address"
        quint8                                   type;
    } GuidStruct;

    enum {
        POS_KEY        = 0,
        POS_GUID       = 1,
        POS_ADDRESS    = 2,
    };
    enum {
        GUID_TYPE_ONE_TABLE,
        GUID_TYPE_SUBTABLE
    };

    GuidClass();
    ~GuidClass ();

    void init(SqlDriver *sqlDriver, const QString& table, quint8 * id, quint8 type = GUID_TYPE_ONE_TABLE);
    QString get_address(quint8 vector_index);
    QString get_guid(quint8 vector_index);
    QString get_subguid(const QString& addr, bool *ok, quint8 vector_index);


private:
    quint8 count;
    QVector<GuidStruct> GuidVector;
    void    next_iterators(quint8 vector_index);
};

#endif
