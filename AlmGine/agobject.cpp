#include "agobject.h"

AGObject::AGObject(QObject *parent)
    : QObject(parent)
{
    m_position = QVector3D(0, 0, 0);
    m_rotation = QVector3D(0, 0, 0);
    m_scale = QVector3D(1, 1, 1);
    m_pivot = QVector3D(0, 0, 0);
}

bool AGObject::visible() const
{
    return m_visible;
}

QVector3D AGObject::position() const
{
    return m_position;
}

QVector3D AGObject::rotation() const
{
    return m_rotation;
}

QVector3D AGObject::scale() const
{
    return m_scale;
}

QVector3D AGObject::pivot() const
{
    return m_pivot;
}

QVector3D AGObject::forward() const
{
    return m_forward;
}

QVector3D AGObject::right() const
{
    return m_right;
}

QVector3D AGObject::up() const
{
    return m_up;
}

QMatrix4x4 AGObject::worldMatrix() const
{
    return m_worldMatrix;
}

void AGObject::setVisible(bool visible)
{
    if (m_visible == visible)
        return;

    m_visible = visible;
    emit visibleChanged();
}

void AGObject::setPosition(const QVector3D &position)
{
    if (m_position == position)
        return;

    m_position = position;
    m_worldMatrixDirty = true;
    emit positionChanged();
}

void AGObject::setRotation(const QVector3D &rotation)
{
    if (m_rotation == rotation)
        return;

    m_rotation = rotation;
    m_worldMatrixDirty = true;
    emit rotationChanged();
}

void AGObject::setScale(const QVector3D &scale)
{
    if (m_scale == scale)
        return;

    m_scale = scale;
    m_worldMatrixDirty = true;
    emit scaleChanged();
}

void AGObject::setPivot(const QVector3D &pivot)
{
    if (m_pivot == pivot)
        return;

    m_pivot = pivot;
    m_worldMatrixDirty = true;
    emit pivotChanged();
}

void AGObject::classBegin()
{
    m_componentComplete = false;
}

void AGObject::componentComplete()
{
    m_componentComplete = true;
}

bool AGObject::isComponentComplete() const
{
    return m_componentComplete;
}

QVector4D AGObject::mapToWorld(const QVector4D &point) const
{
    return point * m_worldMatrix;
}

QVector3D AGObject::mapDirectionToWorld(const QVector3D &dir) const
{
    auto dirMatrix = m_worldMatrix.normalMatrix();
    return mat33::transform(dirMatrix, dir);
}

QVector3D AGObject::mapDirectionFromWorld(const QVector3D &dir) const
{
    auto dirMatrix = mat44::getUpper3x3(m_worldMatrix);
    dirMatrix = dirMatrix.transposed();
    return mat33::transform(dirMatrix, dir);
}

bool AGObject::calculateWorldMatrix()
{
    bool wasDirty = m_worldMatrixDirty;
    m_worldMatrixDirty = false;
    if (wasDirty) {
        QMatrix4x4 transform;
        // offset the origin
        const auto offset = -m_pivot * m_scale;
        // scale
        transform(0, 0) = m_scale.x();
        transform(1, 1) = m_scale.y();
        transform(2, 2) = m_scale.z();
        // offset
        transform(3, 0) = offset.x();
        transform(3, 1) = offset.y();
        transform(3, 2) = offset.z();
        // rotate
        transform = transform * m_rotation.toRotationMatrix();
        // translate
        transform(3, 0) += m_position.x();
        transform(3, 1) += m_position.y();
        transform(3, 2) += m_position.z();
        if (transform != m_worldMatrix)
            m_worldMatrix = transform;
        auto forward = mapDirectionToWorld(QVector3D(0, 0, 1)).normalized();
        auto right = mapDirectionToWorld(QVector3D(1, 0, 0)).normalized();
        auto up = mapDirectionToWorld(QVector3D(0, 1, 0)).normalized();
        if (forward != m_forward) {
            m_forward = forward;
            emit forwardChanged();
        }
        if (right != m_right) {
            m_right = right;
            emit rightChanged();
        }
        if (up != m_up) {
            m_up = up;
            emit upChanged();
        }
    }
    return wasDirty;
}

void AGObject::updateObject(AGViewport *viewport)
{
    Q_UNUSED(viewport);
}
