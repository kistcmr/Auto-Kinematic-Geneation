/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../MainWindow.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[20];
    char stringdata0[440];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 26), // "cameraRotateTriggeredEvent"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 23), // "cameraPanTriggeredEvent"
QT_MOC_LITERAL(4, 63, 24), // "cameraZoomTriggeredEvent"
QT_MOC_LITERAL(5, 88, 25), // "cameraPlusXTriggeredEvent"
QT_MOC_LITERAL(6, 114, 26), // "cameraMinusXTriggeredEvent"
QT_MOC_LITERAL(7, 141, 25), // "cameraPlusYTriggeredEvent"
QT_MOC_LITERAL(8, 167, 26), // "cameraMinusYTriggeredEvent"
QT_MOC_LITERAL(9, 194, 25), // "cameraPlusZTriggeredEvent"
QT_MOC_LITERAL(10, 220, 26), // "cameraMinusZTriggeredEvent"
QT_MOC_LITERAL(11, 247, 20), // "configTriggeredEvent"
QT_MOC_LITERAL(12, 268, 22), // "loadBlocksClickedEvent"
QT_MOC_LITERAL(13, 291, 24), // "saveAssemblyClickedEvent"
QT_MOC_LITERAL(14, 316, 24), // "loadAssemblyClickedEvent"
QT_MOC_LITERAL(15, 341, 25), // "resetAssemblyClickedEvent"
QT_MOC_LITERAL(16, 367, 17), // "headerFileClicked"
QT_MOC_LITERAL(17, 385, 32), // "blockCandidateDoubleClickedEvent"
QT_MOC_LITERAL(18, 418, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(19, 435, 4) // "item"

    },
    "MainWindow\0cameraRotateTriggeredEvent\0"
    "\0cameraPanTriggeredEvent\0"
    "cameraZoomTriggeredEvent\0"
    "cameraPlusXTriggeredEvent\0"
    "cameraMinusXTriggeredEvent\0"
    "cameraPlusYTriggeredEvent\0"
    "cameraMinusYTriggeredEvent\0"
    "cameraPlusZTriggeredEvent\0"
    "cameraMinusZTriggeredEvent\0"
    "configTriggeredEvent\0loadBlocksClickedEvent\0"
    "saveAssemblyClickedEvent\0"
    "loadAssemblyClickedEvent\0"
    "resetAssemblyClickedEvent\0headerFileClicked\0"
    "blockCandidateDoubleClickedEvent\0"
    "QListWidgetItem*\0item"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x08 /* Private */,
       3,    0,   95,    2, 0x08 /* Private */,
       4,    0,   96,    2, 0x08 /* Private */,
       5,    0,   97,    2, 0x08 /* Private */,
       6,    0,   98,    2, 0x08 /* Private */,
       7,    0,   99,    2, 0x08 /* Private */,
       8,    0,  100,    2, 0x08 /* Private */,
       9,    0,  101,    2, 0x08 /* Private */,
      10,    0,  102,    2, 0x08 /* Private */,
      11,    0,  103,    2, 0x08 /* Private */,
      12,    0,  104,    2, 0x08 /* Private */,
      13,    0,  105,    2, 0x08 /* Private */,
      14,    0,  106,    2, 0x08 /* Private */,
      15,    0,  107,    2, 0x08 /* Private */,
      16,    0,  108,    2, 0x08 /* Private */,
      17,    1,  109,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 18,   19,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->cameraRotateTriggeredEvent(); break;
        case 1: _t->cameraPanTriggeredEvent(); break;
        case 2: _t->cameraZoomTriggeredEvent(); break;
        case 3: _t->cameraPlusXTriggeredEvent(); break;
        case 4: _t->cameraMinusXTriggeredEvent(); break;
        case 5: _t->cameraPlusYTriggeredEvent(); break;
        case 6: _t->cameraMinusYTriggeredEvent(); break;
        case 7: _t->cameraPlusZTriggeredEvent(); break;
        case 8: _t->cameraMinusZTriggeredEvent(); break;
        case 9: _t->configTriggeredEvent(); break;
        case 10: _t->loadBlocksClickedEvent(); break;
        case 11: _t->saveAssemblyClickedEvent(); break;
        case 12: _t->loadAssemblyClickedEvent(); break;
        case 13: _t->resetAssemblyClickedEvent(); break;
        case 14: _t->headerFileClicked(); break;
        case 15: _t->blockCandidateDoubleClickedEvent((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
