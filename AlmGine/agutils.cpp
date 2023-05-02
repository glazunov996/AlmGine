#include "agutils.h"

#include <QDebug>

QVector3D mat33::transform(const QMatrix3x3 &m, const QVector3D &v)
{
    const auto r0 = QVector3D(m(0, 0), m(0, 1), m(0, 2));
    const auto r1 = QVector3D(m(1, 0), m(1, 1), m(1, 2));
    const auto r2 = QVector3D(m(2, 0), m(2, 1), m(2, 2));
    return r0 * v.x() + r1 * v.y() + r2 * v.z();
}

QVector3D mat44::transform(const QMatrix4x4 &m, const QVector3D &v)
{
    const auto tmp = mat44::transform(m, QVector4D(v.x(), v.y(), v.z(), 1.0f));
    return QVector3D(tmp.x(), tmp.y(), tmp.z());
}

QVector4D mat44::transform(const QMatrix4x4 &m, const QVector4D &v)
{
    return m.row(0) * v.x() + m.row(1) * v.y() + m.row(2) * v.z() + m.row(3) * v.w();
}

QMatrix3x3 mat44::getUpper3x3(const QMatrix4x4 &m)
{
    const float values[9] = { m(0, 0), m(0, 1), m(0, 2), m(1, 0), m(1, 1), m(1, 2), m(2, 0), m(2, 1), m(2, 2) };
    return QMatrix3x3(values);

}
