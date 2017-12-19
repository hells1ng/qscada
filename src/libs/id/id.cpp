
#include "id.h"

void GuidClass::next_iterators(quint8 vector_index)
{
    if (GuidQueueVector.at(vector_index).isEmpty())
    {
//        qDebug() << "Queue is Empty! Read NORMAL!";

        if (!GuidVector.at(vector_index).main_it->hasNext())
            GuidVector.at(vector_index).main_it->toFront();
        GuidVector[vector_index].current_guid = GuidVector.at(vector_index).main_it->next();

        if (GuidVector.at(vector_index).type == GUID_TYPE_SUBTABLE)
        {
            if (!GuidVector.at(vector_index).sub_it->hasNext())
                GuidVector.at(vector_index).sub_it->toFront();
            GuidVector[vector_index].current_subguid = GuidVector.at(vector_index).sub_it->next();

        }

    }
    else
    {

        mutex_queue->lock();

        qDebug() << "Queue is NOT Empty! Read from QUEUE with size" << GuidQueueVector.at(vector_index).size();

        GuidStructDevice local_guid = GuidQueueVector[vector_index].dequeue();

        GuidVector[vector_index].current_guid = local_guid.main_guid;
        GuidVector[vector_index].current_subguid = local_guid.sub_guid;

        mutex_queue->unlock();

    }
}

bool GuidClass::hasNext(quint8 vector_index)
{
    bool ret = false;
//    QThread::msleep(2500);
    mutex->lock();

    if (GuidVector.at(vector_index).main_it->hasNext())
    {
        GuidActiveStateVector[vector_index] = true;
        ret = true;
    }
    else
    {        
        GuidActiveStateVector[vector_index] = false;
        GuidVector.at(vector_index).main_it->toFront();
        ret = false;
    }
    mutex->unlock();
    return ret;
}

//  get address of device (uniq address in RS485 line for example)
//  and shift iterators
QString GuidClass::get_address(quint8 vector_index)
{
    next_iterators(vector_index);
    return GuidVector.at(vector_index).current_guid.at(POS_ADDRESS);
}

QString GuidClass::get_guid(quint8 vector_index)
{
    return GuidVector.at(vector_index).current_guid.at(POS_GUID);
}

QString GuidClass::get_subguid(const QString& addr, bool* ok, quint8 vector_index)
{
    if (GuidVector.at(vector_index).current_subguid.contains(addr))
        *ok = true;

    return GuidVector.at(vector_index).current_subguid.value(addr);
}

// init guid from table and init id
void GuidClass::init(SqlDriver *sqlDriver, const QString& table, quint8 * id, quint8 type)
{
    GuidStruct  local_guid;
    local_guid.type = GUID_TYPE_ONE_TABLE;

    Data data = sqlDriver->fromGuidTable(table);

    for (quint16 i = 0; i < data.size(); i++) {

        local_guid.main_guid.append(data[i]);
        qDebug() << "Guid Table :" << table << " : " << local_guid.main_guid[i];

        if (type == GUID_TYPE_ONE_TABLE)
        {
            AllGuid.append(data[i].at(POS_GUID));
        }
        else if (type == GUID_TYPE_SUBTABLE) {

            local_guid.type = GUID_TYPE_SUBTABLE;

            const QString subtable = table + "_subguid";

            Data subdata = sqlDriver->fromGuidTable(subtable, data[i].at(POS_KEY));
            QMap<QString, QString> qmap;

            for (quint16 j = 0; j < subdata.size(); j++) {
                qmap.insert(subdata[j].at(POS_ADDRESS), subdata[j].at(POS_GUID));
                AllGuid.append(subdata[j].at(POS_GUID));
            }
            local_guid.sub_guid.append(qmap);
            qDebug() << "   SubGuid Table :" << subtable << " : "<< local_guid.sub_guid[i] << endl;
        }
    }

    //test
    local_guid.id = count;
    local_guid.main_it = new QVectorIterator<QStringList>(local_guid.main_guid);
    local_guid.sub_it = new QVectorIterator<QMap<QString, QString>>(local_guid.sub_guid);

    GuidVector.append(local_guid);
    GuidQueueVector.append(GuidQueue);//create vector of GuidQueue
    GuidActiveStateVector.append(false);

    *id = count;
    count++;
}

bool GuidClass::find_guid(const QString& guid, GuidStructDevice* local_guid)
{
    bool ret = false;
    bool found = false;

    foreach (GuidStruct guidStruct, GuidVector) {
        //если каждому прибору соответствует 1 гуайди
        if (guidStruct.type == GUID_TYPE_ONE_TABLE)
        {
            foreach (QStringList qsl, guidStruct.main_guid)
            {
                if (guid == qsl.at(POS_GUID))
                {
//                    qDebug() << "FIND GUID!" << endl;
                    local_guid->main_guid.append(qsl);
                    local_guid->type = guidStruct.type;
                    local_guid->id = guidStruct.id;

                    found = true;
                    ret = true;
                    break;
                }
                if (found)
                    break;
            }
        }
        //если у 1 прибора несколько гуайди (к примеру счетчик импульсов)
        else if (guidStruct.type == GUID_TYPE_SUBTABLE)
        {
            QMap<QString, QString> qmap;
            int i = 0;
            foreach (qmap, guidStruct.sub_guid)
            {
                if (qmap.values().contains(guid))
                {
//                    qDebug() << "FIND SUBGUID!" << endl;
                    local_guid->main_guid.append(guidStruct.main_guid.at(i));
                    local_guid->sub_guid = guidStruct.sub_guid.at(i);
                    local_guid->type = guidStruct.type;
                    local_guid->id = guidStruct.id;

                    found = true;
                    ret = true;
                    break;
                }
                i++;
                if (found)
                    break;
            }
        }
        if (found)
            break;
    }
    return ret;
}

void GuidClass::addQueue(const QString& guid)
{
    GuidStructDevice local_guid;

    mutex_queue->lock();

    if (find_guid(guid, &local_guid))
    {
//        qDebug() << "Guid::findGuid We are looking for " << guid;
//        qDebug() << "Guid::findGuid guid : " << local_guid.main_guid;
//        qDebug() << "Guid::findGuid subguid : " << local_guid.sub_guid;
//        qDebug() << "Guid::findGuid type : " << local_guid.type;
//        qDebug() << "Guid::findGuid ID : " << local_guid.id << endl;

        GuidQueueVector[local_guid.id].enqueue(local_guid);

        //test code

//        GuidStructDevice local_guid_from_queue =
//                GuidQueueVector[local_guid.id].dequeue();
//        qDebug() << "Guid::findGuid We are looking for " << guid;
//        qDebug() << "Guid::findGuid guid : " << local_guid_from_queue.main_guid;
//        qDebug() << "Guid::findGuid subguid : " << local_guid_from_queue.sub_guid;
//        qDebug() << "Guid::findGuid type : " << local_guid_from_queue.type;
//        qDebug() << "Guid::findGuid ID : " << local_guid_from_queue.id << endl;

    }
    else
    {
        qDebug() << "Guid::findGuid " << guid << " not found!" << endl;
    }

    mutex_queue->unlock();
}

void GuidClass::addQueue(const QStringList guid_qsl)
{
    GuidStructDevice local_guid;
    QVector<GuidStructDevice> local_vector;//вектор для отслеживания повторяющихся гуайди в запросе

    mutex_queue->lock();

    //для каждого гуайди, который нужно добавить в очередь, выполняем:
    foreach (QString guid, guid_qsl) {

        //если он есть в базе
        if (find_guid(guid, &local_guid))
        {
            bool exist = false;
            //сканим вектор на предмет того, есть ли найденный в базе гуайди уже в очереди
            foreach ( GuidStructDevice i, local_vector) {
                if (local_guid.main_guid == i.main_guid)
                {
                    exist = true;
                    break;
                }
            }
            //если гуайди нет в векторе, добавляем в вектор и в очередь
            if (!exist)
            {
                GuidQueueVector[local_guid.id].enqueue(local_guid);
                local_vector.append(local_guid);
            }
        }
        else
        {
            qDebug() << "Guid::findGuid " << guid << " not found!" << endl;
        }
        //чистим локальный гуайди
        local_guid.main_guid.clear();
        local_guid.sub_guid.clear();

    }
    //в конце чистим локальный вектор
    if (!local_vector.isEmpty())
        local_vector.clear();

    mutex_queue->unlock();
}


bool GuidClass::isEmptyQueue(quint8 vector_index)
{
    bool ret = false;

    mutex->lock();

    if (GuidActiveStateVector.at(vector_index))
    {
        ret = true;
    }
    else
        ret =  GuidQueueVector.at(vector_index).isEmpty();

    mutex->unlock();
    return ret;
}

QStringList GuidClass::getAllGuid()
{
    return AllGuid;
}

GuidClass::GuidClass()
{
    mutex = new QMutex;
    mutex_queue = new QMutex;
    count = 0;
}

GuidClass::~GuidClass ()
{
    delete mutex;
    delete mutex_queue;
//    delete it;
//    delete it_sub;
}







