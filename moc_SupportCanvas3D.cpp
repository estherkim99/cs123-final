/****************************************************************************
** Meta object code from reading C++ file 'SupportCanvas3D.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ui/SupportCanvas3D.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SupportCanvas3D.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SupportCanvas3D_t {
    QByteArrayData data[15];
    char stringdata0[252];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SupportCanvas3D_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SupportCanvas3D_t qt_meta_stringdata_SupportCanvas3D = {
    {
QT_MOC_LITERAL(0, 0, 15), // "SupportCanvas3D"
QT_MOC_LITERAL(1, 16, 18), // "aspectRatioChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 13), // "resetUpVector"
QT_MOC_LITERAL(4, 50, 14), // "setCameraAxisX"
QT_MOC_LITERAL(5, 65, 14), // "setCameraAxisY"
QT_MOC_LITERAL(6, 80, 14), // "setCameraAxisZ"
QT_MOC_LITERAL(7, 95, 20), // "setCameraAxonometric"
QT_MOC_LITERAL(8, 116, 23), // "updateCameraHeightAngle"
QT_MOC_LITERAL(9, 140, 23), // "updateCameraTranslation"
QT_MOC_LITERAL(10, 164, 21), // "updateCameraRotationU"
QT_MOC_LITERAL(11, 186, 21), // "updateCameraRotationV"
QT_MOC_LITERAL(12, 208, 21), // "updateCameraRotationN"
QT_MOC_LITERAL(13, 230, 16), // "updateCameraClip"
QT_MOC_LITERAL(14, 247, 4) // "tick"

    },
    "SupportCanvas3D\0aspectRatioChanged\0\0"
    "resetUpVector\0setCameraAxisX\0"
    "setCameraAxisY\0setCameraAxisZ\0"
    "setCameraAxonometric\0updateCameraHeightAngle\0"
    "updateCameraTranslation\0updateCameraRotationU\0"
    "updateCameraRotationV\0updateCameraRotationN\0"
    "updateCameraClip\0tick"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SupportCanvas3D[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   80,    2, 0x0a /* Public */,
       4,    0,   81,    2, 0x0a /* Public */,
       5,    0,   82,    2, 0x0a /* Public */,
       6,    0,   83,    2, 0x0a /* Public */,
       7,    0,   84,    2, 0x0a /* Public */,
       8,    0,   85,    2, 0x0a /* Public */,
       9,    0,   86,    2, 0x0a /* Public */,
      10,    0,   87,    2, 0x0a /* Public */,
      11,    0,   88,    2, 0x0a /* Public */,
      12,    0,   89,    2, 0x0a /* Public */,
      13,    0,   90,    2, 0x0a /* Public */,
      14,    0,   91,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

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

       0        // eod
};

void SupportCanvas3D::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SupportCanvas3D *_t = static_cast<SupportCanvas3D *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->aspectRatioChanged(); break;
        case 1: _t->resetUpVector(); break;
        case 2: _t->setCameraAxisX(); break;
        case 3: _t->setCameraAxisY(); break;
        case 4: _t->setCameraAxisZ(); break;
        case 5: _t->setCameraAxonometric(); break;
        case 6: _t->updateCameraHeightAngle(); break;
        case 7: _t->updateCameraTranslation(); break;
        case 8: _t->updateCameraRotationU(); break;
        case 9: _t->updateCameraRotationV(); break;
        case 10: _t->updateCameraRotationN(); break;
        case 11: _t->updateCameraClip(); break;
        case 12: _t->tick(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SupportCanvas3D::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SupportCanvas3D::aspectRatioChanged)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject SupportCanvas3D::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_SupportCanvas3D.data,
      qt_meta_data_SupportCanvas3D,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SupportCanvas3D::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SupportCanvas3D::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SupportCanvas3D.stringdata0))
        return static_cast<void*>(const_cast< SupportCanvas3D*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int SupportCanvas3D::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void SupportCanvas3D::aspectRatioChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
