/****************************************************************************
** Meta object code from reading C++ file 'ex6.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SRGGE/ex6.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ex6.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ex6_t {
    QByteArrayData data[9];
    char stringdata0[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ex6_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ex6_t qt_meta_stringdata_ex6 = {
    {
QT_MOC_LITERAL(0, 0, 3), // "ex6"
QT_MOC_LITERAL(1, 4, 12), // "SetFramerate"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 15), // "setNumberCopies"
QT_MOC_LITERAL(4, 34, 11), // "startMuseum"
QT_MOC_LITERAL(5, 46, 15), // "initializeWorld"
QT_MOC_LITERAL(6, 62, 17), // "computeVisibility"
QT_MOC_LITERAL(7, 80, 11), // "setNotSuper"
QT_MOC_LITERAL(8, 92, 8) // "setSuper"

    },
    "ex6\0SetFramerate\0\0setNumberCopies\0"
    "startMuseum\0initializeWorld\0"
    "computeVisibility\0setNotSuper\0setSuper"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ex6[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   52,    2, 0x0a /* Public */,
       4,    0,   55,    2, 0x0a /* Public */,
       5,    0,   56,    2, 0x0a /* Public */,
       6,    0,   57,    2, 0x0a /* Public */,
       7,    0,   58,    2, 0x0a /* Public */,
       8,    0,   59,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ex6::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ex6 *_t = static_cast<ex6 *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SetFramerate((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->setNumberCopies((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->startMuseum(); break;
        case 3: _t->initializeWorld(); break;
        case 4: _t->computeVisibility(); break;
        case 5: _t->setNotSuper(); break;
        case 6: _t->setSuper(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ex6::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ex6::SetFramerate)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ex6::staticMetaObject = {
    { &Viewer1::staticMetaObject, qt_meta_stringdata_ex6.data,
      qt_meta_data_ex6,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ex6::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ex6::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ex6.stringdata0))
        return static_cast<void*>(this);
    return Viewer1::qt_metacast(_clname);
}

int ex6::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Viewer1::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ex6::SetFramerate(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
