
#include "id.h"

void GuidClass::next_iterators(quint8 vector_index)
{
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
        qDebug() << "Guid Table :" << table << " : "<< local_guid.main_guid[i];


        if (type == GUID_TYPE_SUBTABLE) {

            local_guid.type = GUID_TYPE_SUBTABLE;

            const QString subtable = table + "_subguid";

            Data subdata = sqlDriver->fromGuidTable(subtable, data[i].at(POS_KEY));
            QMap<QString, QString> qmap;

            for (quint16 j = 0; j < subdata.size(); j++) {
                qmap.insert(subdata[j].at(POS_ADDRESS), subdata[j].at(POS_GUID));
            }
            local_guid.sub_guid.append(qmap);
            qDebug() << "   SubGuid Table :" << subtable << " : "<< local_guid.sub_guid[i] << endl;
        }
    }

    //test
    local_guid.main_it = new QVectorIterator<QStringList>(local_guid.main_guid);
    local_guid.sub_it = new QVectorIterator<QMap<QString, QString>>(local_guid.sub_guid);

    GuidVector.append(local_guid);
    *id = count;
    count++;
}


GuidClass::GuidClass()
{
    count = 0;
}

GuidClass::~GuidClass ()
{
//    delete it;
//    delete it_sub;
}







