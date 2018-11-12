/****************************************************************************
** Meta object code from reading C++ file 'StateMachine.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../StateMachine.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StateMachine.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_StateMachine_t {
    QByteArrayData data[28];
    char stringdata0[379];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_StateMachine_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_StateMachine_t qt_meta_stringdata_StateMachine = {
    {
QT_MOC_LITERAL(0, 0, 12), // "StateMachine"
QT_MOC_LITERAL(1, 13, 8), // "addBlock"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 17), // "romas::BlockInfo&"
QT_MOC_LITERAL(4, 41, 10), // "block_info"
QT_MOC_LITERAL(5, 52, 14), // "cancelAssemble"
QT_MOC_LITERAL(6, 67, 14), // "goNextAssemble"
QT_MOC_LITERAL(7, 82, 18), // "goPreviousAssemble"
QT_MOC_LITERAL(8, 101, 14), // "finishAssemble"
QT_MOC_LITERAL(9, 116, 10), // "pickParent"
QT_MOC_LITERAL(10, 127, 13), // "romas::Block*"
QT_MOC_LITERAL(11, 141, 5), // "block"
QT_MOC_LITERAL(12, 147, 8), // "joint_ID"
QT_MOC_LITERAL(13, 156, 9), // "pickChild"
QT_MOC_LITERAL(14, 166, 17), // "changeParentAxis1"
QT_MOC_LITERAL(15, 184, 11), // "romas::AXIS"
QT_MOC_LITERAL(16, 196, 4), // "axis"
QT_MOC_LITERAL(17, 201, 16), // "changeChildAxis1"
QT_MOC_LITERAL(18, 218, 17), // "changeParentAxis2"
QT_MOC_LITERAL(19, 236, 16), // "changeChildAxis2"
QT_MOC_LITERAL(20, 253, 19), // "changeZRotationAxis"
QT_MOC_LITERAL(21, 273, 19), // "changeXRotationAxis"
QT_MOC_LITERAL(22, 293, 16), // "setRevoluteJoint"
QT_MOC_LITERAL(23, 310, 17), // "setPrismaticJoint"
QT_MOC_LITERAL(24, 328, 12), // "loadAssembly"
QT_MOC_LITERAL(25, 341, 12), // "saveAssembly"
QT_MOC_LITERAL(26, 354, 13), // "resetAssembly"
QT_MOC_LITERAL(27, 368, 10) // "headerFile"

    },
    "StateMachine\0addBlock\0\0romas::BlockInfo&\0"
    "block_info\0cancelAssemble\0goNextAssemble\0"
    "goPreviousAssemble\0finishAssemble\0"
    "pickParent\0romas::Block*\0block\0joint_ID\0"
    "pickChild\0changeParentAxis1\0romas::AXIS\0"
    "axis\0changeChildAxis1\0changeParentAxis2\0"
    "changeChildAxis2\0changeZRotationAxis\0"
    "changeXRotationAxis\0setRevoluteJoint\0"
    "setPrismaticJoint\0loadAssembly\0"
    "saveAssembly\0resetAssembly\0headerFile"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_StateMachine[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  109,    2, 0x0a /* Public */,
       5,    0,  112,    2, 0x0a /* Public */,
       6,    0,  113,    2, 0x0a /* Public */,
       7,    0,  114,    2, 0x0a /* Public */,
       8,    0,  115,    2, 0x0a /* Public */,
       9,    2,  116,    2, 0x0a /* Public */,
      13,    2,  121,    2, 0x0a /* Public */,
      14,    1,  126,    2, 0x0a /* Public */,
      17,    1,  129,    2, 0x0a /* Public */,
      18,    1,  132,    2, 0x0a /* Public */,
      19,    1,  135,    2, 0x0a /* Public */,
      20,    1,  138,    2, 0x0a /* Public */,
      21,    1,  141,    2, 0x0a /* Public */,
      22,    0,  144,    2, 0x0a /* Public */,
      23,    0,  145,    2, 0x0a /* Public */,
      24,    0,  146,    2, 0x0a /* Public */,
      25,    0,  147,    2, 0x0a /* Public */,
      26,    0,  148,    2, 0x0a /* Public */,
      27,    0,  149,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10, QMetaType::Int,   11,   12,
    QMetaType::Void, 0x80000000 | 10, QMetaType::Int,   11,   12,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void StateMachine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        StateMachine *_t = static_cast<StateMachine *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->addBlock((*reinterpret_cast< romas::BlockInfo(*)>(_a[1]))); break;
        case 1: _t->cancelAssemble(); break;
        case 2: _t->goNextAssemble(); break;
        case 3: _t->goPreviousAssemble(); break;
        case 4: _t->finishAssemble(); break;
        case 5: _t->pickParent((*reinterpret_cast< romas::Block*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->pickChild((*reinterpret_cast< romas::Block*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->changeParentAxis1((*reinterpret_cast< romas::AXIS(*)>(_a[1]))); break;
        case 8: _t->changeChildAxis1((*reinterpret_cast< romas::AXIS(*)>(_a[1]))); break;
        case 9: _t->changeParentAxis2((*reinterpret_cast< romas::AXIS(*)>(_a[1]))); break;
        case 10: _t->changeChildAxis2((*reinterpret_cast< romas::AXIS(*)>(_a[1]))); break;
        case 11: _t->changeZRotationAxis((*reinterpret_cast< romas::AXIS(*)>(_a[1]))); break;
        case 12: _t->changeXRotationAxis((*reinterpret_cast< romas::AXIS(*)>(_a[1]))); break;
        case 13: _t->setRevoluteJoint(); break;
        case 14: _t->setPrismaticJoint(); break;
        case 15: _t->loadAssembly(); break;
        case 16: _t->saveAssembly(); break;
        case 17: _t->resetAssembly(); break;
        case 18: _t->headerFile(); break;
        default: ;
        }
    }
}

const QMetaObject StateMachine::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_StateMachine.data,
      qt_meta_data_StateMachine,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *StateMachine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StateMachine::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_StateMachine.stringdata0))
        return static_cast<void*>(const_cast< StateMachine*>(this));
    return QObject::qt_metacast(_clname);
}

int StateMachine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 19;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
