
#include "id.h"

void GuidClass::set_index(quint16 index)
{
    _index = index;
}
QString GuidClass::get_guid()
{
    return _guid[_index].at(POS_GUID);
}

QString GuidClass::get_subguid(const QString& addr, bool* ok)
{
    if (_subguid[_index].contains(addr))
            *ok = true;
    return _subguid[_index].value(addr);
}

QString GuidClass::get_address()
{
    return _guid[_index].at(POS_ADDRESS);
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

            const QString subtable = _guid[i].at(POS_GUID);

            Data subdata = sqlDriver->fromGuidTable(subtable);
            QMap<QString, QString> qmap;

            for (quint16 j = 0; j < subdata.size(); j++) {
                qmap.insert(subdata[j].at(POS_ADDRESS), subdata[j].at(POS_GUID));
            }
            _subguid.append(qmap);
            qDebug() << "   SubGuid Table :" << subtable << " : "<< _subguid[i] << endl;
        }
    }
}







