/****************************************************************************
** Meta object code from reading C++ file 'MainLoopModule.h'
**
** Created: Wed 30. Oct 09:08:46 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MainLoopModule.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainLoopModule.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainLoopModule[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   16,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      38,   16,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainLoopModule[] = {
    "MainLoopModule\0\0p\0AsyncMessage(void*)\0"
    "Async2SyncMessage(void*)\0"
};

void MainLoopModule::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainLoopModule *_t = static_cast<MainLoopModule *>(_o);
        switch (_id) {
        case 0: _t->AsyncMessage((*reinterpret_cast< void*(*)>(_a[1]))); break;
        case 1: _t->Async2SyncMessage((*reinterpret_cast< void*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainLoopModule::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainLoopModule::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MainLoopModule,
      qt_meta_data_MainLoopModule, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainLoopModule::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainLoopModule::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainLoopModule::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainLoopModule))
        return static_cast<void*>(const_cast< MainLoopModule*>(this));
    return QObject::qt_metacast(_clname);
}

int MainLoopModule::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void MainLoopModule::AsyncMessage(void * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
