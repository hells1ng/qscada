
#include "id.h"

void GuidClass::set_index(quint16 index)
{
    _index = index;
}
QString GuidClass::get_guid()
{
//    return _guid[_index].at(POS_GUID);
    return current_guid.at(POS_GUID);
}

QString GuidClass::get_subguid(const QString& addr, bool* ok)
{

    if (current_subguid.contains(addr))
            *ok = true;
    return current_subguid.value(addr);
}

//  get address of device (uniq address in RS485 line for example)
//  and shift iterators
QString GuidClass::get_address()
{
    next_iterators();
//    qDebug() << "GUID = " << current_guid;
//    qDebug() << "SUBGUID = " << current_subguid;
    return current_guid.at(POS_ADDRESS);
}

void GuidClass::next_iterators()
{
    if (it->hasNext())
        current_guid = it->next();
    else
    {
        it->toFront();
        current_guid = it->next();
    }

    if (_guidtype == GUID_TYPE_SUBTABLE) {
        if (it_sub->hasNext())
            current_subguid = it_sub->next();
        else
        {
            it_sub->toFront();
            current_subguid = it_sub->next();
        }
    }
}

quint16 GuidClass::size()
{
    return _guid.size();
}

void GuidClass::add(QStringList qsl)
{
    _guid.append(qsl);
}

void GuidClass::init(SqlDriver *sqlDriver, const QString& table)
{

    Data data = sqlDriver->fromGuidTable(table);

    for (quint16 i = 0; i < data.size(); i++) {

        _guid.append(data[i]);
        qDebug() << "Guid Table :" << table << " : "<< _guid[i];


        if (_guidtype == GUID_TYPE_SUBTABLE) {

//            const QString subtable = _guid[i].at(POS_GUID);
            const QString subtable = table + "_subguid";

//            Data subdata = sqlDriver->fromGuidTable(subtable);
            Data subdata = sqlDriver->fromGuidTable(subtable, data[i].at(POS_KEY));
            QMap<QString, QString> qmap;

            for (quint16 j = 0; j < subdata.size(); j++) {
                qmap.insert(subdata[j].at(POS_ADDRESS), subdata[j].at(POS_GUID));
            }
            _subguid.append(qmap);
            qDebug() << "   SubGuid Table :" << subtable << " : "<< _subguid[i] << endl;
        }
    }

    //test
    it = new QVectorIterator<QStringList>(_guid);
    it_sub = new QVectorIterator<QMap<QString, QString>>(_subguid);
}

GuidClass::GuidClass(quint8 guid_type) :
    _guidtype(guid_type)
{

}

GuidClass::~GuidClass () {
    delete it;
    delete it_sub;
}

bool GuidClass::hasNext()
{
    bool ret = true;
    if (!it->hasNext())
//        qDebug() << "GuidClass::hasNext : " << it->next();
    {
//        it->toFront();
        ret = false;
    }
    return ret;
}





