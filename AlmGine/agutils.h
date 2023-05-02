#pragma once

#include <QVector3D>
#include <QMatrix3x3>
#include <QMatrix4x4>

inline QVector3D degToRad(const QVector3D &v) {
    return QVector3D(qDegreesToRadians(v.x()), qDegreesToRadians(v.y()), qDegreesToRadians(v.z()));
}

inline QVector3D radToDeg(const QVector3D &v) {
    return QVector3D(qRadiansToDegrees(v.x()), qRadiansToDegrees(v.y()), qRadiansToDegrees(v.z()));
}

namespace mat33 {
QVector3D transform(const QMatrix3x3 &m, const QVector3D &v);
}

namespace mat44 {
QVector3D transform(const QMatrix4x4 &m, const QVector3D &v);
QVector4D transform(const QMatrix4x4 &m, const QVector4D &v);
QMatrix3x3 getUpper3x3(const QMatrix4x4 &m);
}

class Rotation
{
public:
    Rotation() = default;
    explicit Rotation(const QVector3D &r)
        : m_rotation(r)
    {}

    Rotation &operator=(const QVector3D &r) noexcept
    {
        m_rotation = r;
        m_dirty = true;
        return *this;
    }

    friend inline bool operator ==(const Rotation &a, QVector3D &r)
    {
        return qFuzzyCompare(QVector3D(a), r);
    }

    friend inline bool operator !=(const Rotation &a, QVector3D &r)
    {
        return !(a == r);
    }

    [[nodiscard]] inline operator QVector3D() const { return m_rotation; }

    QMatrix4x4 toRotationMatrix() const
    {
        if (m_dirty) {
            const auto rot = degToRad(m_rotation);
            const auto cx = qCos(rot.x());
            const auto sx = qSin(rot.x());
            const auto cy = qCos(rot.y());
            const auto sy = qSin(rot.y());
            const auto cz = qCos(rot.z());
            const auto sz = qSin(rot.z());
            const auto mx = QMatrix4x4(
                1, 0, 0, 0,
                0, cx, sx, 0,
                0, -sx, cx, 0,
                0, 0, 0, 1
            );
            const auto my = QMatrix4x4(
                cy, 0, -sy, 0,
                0, 1, 0, 0,
                sy, 0, cy, 0,
                0, 0, 0, 1
            );
            const auto mz = QMatrix4x4(
                cz, sz, 0, 0,
                -sz, cz, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            );
            m_rotationMatrix = mx * my * mz;
            m_dirty = false;
        }
        return m_rotationMatrix;
    }

private:
    mutable QVector3D m_rotation;
    mutable QMatrix4x4 m_rotationMatrix;
    mutable bool m_dirty;
};
