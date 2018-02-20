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
    //структура для одной линии RS485
    typedef struct
    {
        QVector<QStringList>                     main_guid;  //vector of qsl for pair "guid - device adress"
        QVector<QMap<QString,QString>>           sub_guid;   //vector of map for pair "subguid - pin address of device"
        QVectorIterator<QStringList>*            main_it;
        QVectorIterator<QMap<QString, QString>>* sub_it;
        QStringList                              current_guid;    //qsl for pair "guid - address"
        QMap<QString, QString>                   current_subguid; //map for pair "subguid - pin address"
        quint8                                   type;
        quint8                                   id;    //id of this struct
    } GuidStruct;

    //структура для одного прибора
    typedef struct
    {
        QStringList             main_guid;
        QMap<QString, QString>  sub_guid;
        quint8                  type;
        quint8                  id;
    } GuidStructDevice;

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

    int init(SqlDriver *sqlDriver, const QString& table, quint8 * id, quint8 type = GUID_TYPE_ONE_TABLE);
    bool hasNext(quint8 vector_index);
    QString get_address(quint8 vector_index);
    QString get_guid(quint8 vector_index);
    QString get_subguid(const QString& addr, bool *ok, quint8 vector_index);

    bool    isEmptyQueue(quint8 vector_index);
    void    addQueue(const QString &guid);
    void    addQueue(const QStringList guid_qsl);
    QStringList getAllGuid();

private:
    quint8                              count;
    QVector<GuidStruct>                 GuidVector;
    QVector<QQueue<GuidStructDevice>>   GuidQueueVector;
    QVector<bool>                       GuidActiveStateVector;
    QStringList                         AllGuid;
//    QVector<QVectorIterator<QStringList>> MainItVector;
//    QVector<QVectorIterator<QMap<QString, QString>>> SubItVector;
    QMutex*                             mutex;
    QMutex*                             mutex2;
    QMutex*                             mutex_queue;


    QQueue<GuidStructDevice> GuidQueue; //queue of deviceces for reading
    void    next_iterators(quint8 vector_index);
    bool    find_guid(const QString& guid, GuidStructDevice* local_guid);
};

#endif
