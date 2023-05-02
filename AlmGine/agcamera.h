#pragma once

#include <QMatrix4x4>

#include "agobject.h"

class AGCamera : public AGObject
{
    Q_OBJECT
    Q_PROPERTY(qreal fov READ fov WRITE setFov NOTIFY fovChanged)
    Q_PROPERTY(qreal clipNear READ clipNear WRITE setClipNear NOTIFY clipNearChanged)
    Q_PROPERTY(qreal clipFar READ clipFar WRITE setClipFar NOTIFY clipFarChanged)

public:
    AGCamera(QQuickItem *parent = nullptr);

    qreal fov() const;
    qreal clipNear() const;
    qreal clipFar() const;

    qreal zFactor() const;

    Q_INVOKABLE QVector4D mapToCamera(const QVector4D &point) const;
    Q_INVOKABLE QVector4D mapToViewport(const QVector4D &point) const;
    Q_INVOKABLE bool calculateViewProjectionMatrix(const QRectF &inViewport);

public Q_SLOTS:
    void setFov(qreal fov);
    void setClipNear(qreal clipNear);
    void setClipFar(qreal clipFar);

Q_SIGNALS:
    void fovChanged();
    void clipNearChanged();
    void clipFarChanged();

private:
    qreal m_fov = 90.0;
    qreal m_clipNear = 10;
    qreal m_clipFar = 10000.0;
    QRectF m_viewport;
    QMatrix4x4 m_projectViewMatrix;
    qreal m_zFactor = 0;
    bool m_cameraDirty = true;
};
