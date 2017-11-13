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
    typedef QVector<QStringList> Guid;
    typedef QVector<QMap<QString, QString>> SubGuid;
    enum {
        POS_KEY        = 0,
        POS_GUID       = 1,
        POS_ADDRESS    = 2,
    };
    enum TypeOfGuid {
        GUID_TYPE_ONE_TABLE,
        GUID_TYPE_SUBTABLE
    };

    GuidClass(quint8 guid_type = TypeOfGuid::GUID_TYPE_ONE_TABLE) :
        _guidtype(guid_type) {

    }

    ~GuidClass () {

    }

    void    set_index(quint16 index);
    QString get_guid();
    QString get_address();
    QString get_subguid(const QString& addr, bool *ok);
    quint16  size();
    void init(SqlDriver *sqlDriver, const QString& table);

private:
    Guid    _guid;
    SubGuid _subguid;
    quint16 _index;
    quint8  _guidtype;
    void add(QStringList qsl);
};

#endif
