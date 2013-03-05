/****************************************************************************
** Meta object code from reading C++ file 'ModBroadcast.hpp'
**
** Created: Tue Mar 5 18:24:57 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ModBroadcast.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ModBroadcast.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ModBroadcast[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      47,   36,   13,   13, 0x0a,
      80,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ModBroadcast[] = {
    "ModBroadcast\0\0broadcast(QByteArray)\0"
    "entry,sock\0incomingTcp(QString,QTcpSocket*)\0"
    "incoming(QByteArray)\0"
};

void ModBroadcast::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ModBroadcast *_t = static_cast<ModBroadcast *>(_o);
        switch (_id) {
        case 0: _t->broadcast((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 1: _t->incomingTcp((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QTcpSocket*(*)>(_a[2]))); break;
        case 2: _t->incoming((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ModBroadcast::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ModBroadcast::staticMetaObject = {
    { &Daemon::staticMetaObject, qt_meta_stringdata_ModBroadcast,
      qt_meta_data_ModBroadcast, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ModBroadcast::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ModBroadcast::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ModBroadcast::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ModBroadcast))
        return static_cast<void*>(const_cast< ModBroadcast*>(this));
    return Daemon::qt_metacast(_clname);
}

int ModBroadcast::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Daemon::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void ModBroadcast::broadcast(const QByteArray & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
