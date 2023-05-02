#include "agcamera.h"

AGCamera::AGCamera(QQuickItem *parent) :
    AGObject(parent)
{
}

qreal AGCamera::fov() const
{
    return m_fov;
}

qreal AGCamera::clipNear() const
{
    return m_clipNear;
}

qreal AGCamera::clipFar() const
{
    return m_clipFar;
}

qreal AGCamera::zFactor() const
{
    return m_zFactor;
}

void AGCamera::setFov(qreal fov)
{
    if (qFuzzyCompare(fov, m_fov))
        return;
    m_fov = fov;
    m_cameraDirty = true;
    emit fovChanged();
}

void AGCamera::setClipNear(qreal clipNear)
{
    if (qFuzzyCompare(clipNear, m_clipNear))
        return;
    m_clipNear = clipNear;
    m_cameraDirty = true;
    emit clipNearChanged();
}

void AGCamera::setClipFar(qreal clipFar)
{
    if (qFuzzyCompare(clipFar, m_clipFar))
        return;
    m_clipFar = clipFar;
    m_cameraDirty = true;
    emit clipFarChanged();
}

QVector4D AGCamera::mapToCamera(const QVector4D &point) const
{
    return point * m_projectViewMatrix;
}

QVector4D AGCamera::mapToViewport(const QVector4D &point) const
{
    auto v = point;
    const auto a = (0.5 * m_viewport.width() - 0.5);
    const auto b = (0.5 * m_viewport.height() - 0.5);
    v.setX(a + a * v.x() / v.z());
    v.setY(b - b * v.y() / v.z());
    return v;
}

bool AGCamera::calculateViewProjectionMatrix(const QRectF &inViewport)
{
    const bool viewportChanged = inViewport != m_viewport;
    const bool dirty = calculateWorldMatrix();
    if (!viewportChanged && !m_cameraDirty && !dirty)
        return false;
    m_cameraDirty = false;
    m_viewport = inViewport;
    const qreal aspectRatio = m_viewport.width() / m_viewport.height();
    const qreal viewDistance = qTan(qDegreesToRadians(m_fov / 2.0));
    m_zFactor = 1.0 / viewDistance;
    const auto projection = QMatrix4x4(
        viewDistance, 0, 0, 0,
        0, viewDistance * aspectRatio, 0, 0,
        0, 0, 1, 1,
        0, 0, 0, 0
    );
    m_projectViewMatrix = worldMatrix().inverted() * projection;
    return true;
}
